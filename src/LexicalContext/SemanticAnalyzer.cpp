//
// Created by henry on 2022-04-30.
//

#include "SemanticAnalyzer.h"

#include "ASTDeclaration.h"
#include "ASTExpression.h"
#include "ASTStatement.h"
#include "ASTLiteral.h"
#include "ASTContext.h"

#include "LexicalScope.h"
#include "TypeContext.h"

namespace reflex {

ReferenceError::ReferenceError(const std::string &arg)
    : runtime_error("ReferenceError: " + arg) {}

AnalysisError::AnalysisError(const std::string &arg)
    : runtime_error("AnalysisError: " + arg) {}

Declaration *SymbolTable::find(const std::string &name) {
    if (symbolTable.contains(name)) return symbolTable.at(name);
    if (isGlobalTable()) {
        throw ReferenceError{"Cannot find reference to " + name};
    }
    auto decl = parent->find(name); // captures from parent scope
    captures.emplace(decl);
    return decl;
}

void SymbolTable::add(Declaration *decl, const std::string &name) {
    auto declname = name.empty() ? decl->getDeclname() : name;
    if (symbolTable.contains(declname)) {
        throw ReferenceError{declname + " already exists in current scope"};
    }
    symbolTable[declname] = decl;
}

void SemanticAnalyzer::analyzeFunction(FunctionDecl *decl) {
    if (symbolTables.empty()) {
        symbolTables.push(std::make_unique<SymbolTable>(nullptr));
    } else {
        symbolTables.push(std::make_unique<SymbolTable>(symbolTables.top().get()));
    }
    lexicalScopes.push(decl->getScope()->getChild());

    auto &scope = symbolTables.top();
    for (auto param: decl->getParamDecls()) {
        scope->add(param);
    }

    if (decl->getBody()) {
        auto stmts = decl->getBody();
        for (auto stmt: stmts->getStmts()) {
            stmt->accept(this);
        }
    }

    lexicalScopes.pop();
    symbolTables.pop();
}

void SemanticAnalyzer::analyzeMethod(MethodDecl *decl) {
    symbolTables.push(std::make_unique<SymbolTable>(nullptr));
    lexicalScopes.push(decl->getParent()->getScope()->getChild());

    auto &scope = symbolTables.top();
    auto klass = dynamic_cast<ClassDecl *>(decl->getParent());

    auto thisDecl = std::make_unique<VariableDecl>(klass->location(), "this", nullptr);
    auto thisRefType = typeContext.getReferenceType(dynamic_cast<ReferenceableType *>(klass->getType()));
    thisDecl->setType(thisRefType);

    scope->add(thisDecl.get(), "this");
    for (auto field: klass->getFields()) {
        scope->add(field);
    }

    analyzeFunction(decl);

    lexicalScopes.pop();
    symbolTables.pop();
}

void SemanticAnalyzer::analyzeLambda(FunctionLiteral *literal) {
    symbolTables.push(std::make_unique<SymbolTable>(symbolTables.top().get()));
    lexicalScopes.push(literal->getScope()->getChild());

    auto &scope = symbolTables.top();
    for (auto param: literal->getParamDecls()) {
        scope->add(param);
    }

    if (literal->getBody()) {
        auto stmts = literal->getBody();
        for (auto stmt: stmts->getStmts()) {
            stmt->accept(this);
        }
    }

    lexicalScopes.pop();
    symbolTables.pop();
}

void SemanticAnalyzer::analyzeField(FieldDecl *decl) {
    symbolTables.push(std::make_unique<SymbolTable>(nullptr));
    lexicalScopes.push(decl->getParent()->getScope()->getChild());

    if (decl->getInitializer()) {
        decl->getInitializer()->accept(&exprAnalysisPass);
    }

    lexicalScopes.pop();
    symbolTables.pop();
}

OpaqueType SemanticAnalyzer::visit(BlockStmt &stmt) {
    symbolTables.push(std::make_unique<SymbolTable>(symbolTables.top().get()));
    lexicalScopes.push(stmt.getScope()->getChild());

    for (auto s: stmt.getStmts()) {
        s->accept(this);
    }

    lexicalScopes.pop();
    symbolTables.pop();
    return {};
}

OpaqueType SemanticAnalyzer::visit(DeclStmt &stmt) {
    auto decl = stmt.getDecl();
    if (auto var = dynamic_cast<VariableDecl *>(decl)) {
        auto type = typeParser.parseReferenceTypeExpr(var->getTypeDecl(), lexicalScopes.top());
        var->setType(type);
        decl->setType(type);
        symbolTables.top()->add(decl);

        if (var->getInitializer()) {
            auto rvalue = Generic<Expression *>::Get(var->getInitializer()->accept(&exprAnalysisPass));
            auto declType = typeParser.parseReferenceTypeExpr(var->getTypeDecl(), lexicalScopes.top());
            var->setInitializer(exprAnalysisPass.insertImplicitCast(rvalue, declType));
        }
    }
    return {};
}

OpaqueType SemanticAnalyzer::visit(BreakStmt &stmt) {
    if (!inloop) throw AnalysisError{"BreakStmt is not contained in a loop"};
    return {};
}

OpaqueType SemanticAnalyzer::visit(ContinueStmt &stmt) {
    if (!inloop) throw AnalysisError{"ContinueStmt is not contained in a loop"};
    return {};
}

OpaqueType SemanticAnalyzer::visit(ReturnStmt &stmt) {
    if (stmt.getReturnValue()) {
        auto expr = Generic<Expression *>::Get(stmt.getReturnValue()->accept(&exprAnalysisPass));
        stmt.setReturnValue(expr);
        stmt.setReturnType(expr->getType());
    } else {
        stmt.setReturnType(typeContext.getVoidType());
    }
    return {};
}

OpaqueType SemanticAnalyzer::visit(IfStmt &stmt) {
    // visit conditionals
    stmt.getPrimaryBlock()->accept(this);
    if (stmt.getElseBlock()) stmt.getElseBlock()->accept(this);
    return {};
}

OpaqueType SemanticAnalyzer::visit(ForStmt &stmt) {
    // visit range clause
    inloop = true;
    stmt.getBody()->accept(this);
    inloop = false;
    return {};
}

OpaqueType SemanticAnalyzer::visit(WhileStmt &stmt) {
    // visit cond
    inloop = true;
    stmt.getBody()->accept(this);
    inloop = false;
    return {};
}

OpaqueType SemanticAnalyzer::visit(AssignmentStmt &stmt) {
    auto lvalue = Generic<Expression *>::Get(stmt.getLhs()->accept(&exprAnalysisPass));
    auto rvalue = Generic<Expression *>::Get(stmt.getRhs()->accept(&exprAnalysisPass));
    stmt.setLhs(lvalue);
    if (stmt.getAssignOp() != Operator::AssignOperator::Equal) throw TypeError{"Unsupported assignment operator"};
    // lhs must be DeclRefExpr or ArrayIndexExpr to be assignable
    if (auto declLValue = dynamic_cast<DeclRefExpr *>(lvalue)) {
        auto lvalueType = declLValue->getType();
        auto convertedRValue = exprAnalysisPass.insertImplicitCast(rvalue, lvalueType);
        stmt.setRhs(convertedRValue);
        return {};
    } else if (auto indexLValue = dynamic_cast<IndexExpr *>(lvalue)) {
        auto indexedType = indexLValue->getType();
        auto convertedRValue = exprAnalysisPass.insertImplicitCast(rvalue, indexedType);
        stmt.setRhs(convertedRValue);
        return {};
    }
    stmt.setRhs(rvalue); // default new state
    throw TypeError{
        "Cannot assign " + rvalue->getType()->getTypeString() +
            " to type " + lvalue->getType()->getTypeString()
    };
}

OpaqueType SemanticAnalyzer::visit(ExpressionStmt &stmt) {
    stmt.setExpr(Generic<Expression *>::Get(
        stmt.getExpr()->accept(&exprAnalysisPass))
    );
    return {};
}

OpaqueType ExpressionAnalyzer::visit(DeclRefExpr &expr) {
    auto name = expr.getReferenceName();
    try { // resolve in local scope
        auto decl = parent.symbolTables.top()->find(name);
        expr.setType(decl->getType());
        expr.setDecl(decl);
    } catch (ReferenceError &_) {
        try { // resolve in lexical scope
            auto member = parent.lexicalScopes.top()->resolve(name);
            expr.setType(member->getMemberType());
            expr.setDecl(dynamic_cast<Declaration *>(member->getParent()->getNodeDecl()));
        } catch (LexicalError &err) {
            throw ReferenceError{err.what()};
        }
    }
    return Generic<Expression *>::Create(&expr);
}

OpaqueType ExpressionAnalyzer::visit(NewExpr &expr) {
    return Generic<Expression *>::Create(&expr);
}

OpaqueType ExpressionAnalyzer::visit(NumberLiteral &literal) {
    if (static_cast<int>(literal.getValue()) == literal.getValue()) {
        literal.setType(typeContext.getBuiltinType(BuiltinType::Integer));
    } else {
        literal.setType(typeContext.getBuiltinType(BuiltinType::Number));
    }
    return Generic<Expression *>::Create(&literal);
}

OpaqueType ExpressionAnalyzer::visit(StringLiteral &literal) {
    return {}; // todo: implement string
}

OpaqueType ExpressionAnalyzer::visit(BooleanLiteral &literal) {
    literal.setType(typeContext.getBuiltinType(BuiltinType::Boolean));
    return Generic<Expression *>::Create(&literal);
}

OpaqueType ExpressionAnalyzer::visit(NullLiteral &literal) {
    literal.setType(typeContext.getReferenceType(nullptr));
    return Generic<Expression *>::Create(&literal);
}

OpaqueType ExpressionAnalyzer::visit(UnaryExpr &expr) {
    auto subexpr = Generic<Expression *>::Get(expr.getExpr()->accept(this));
    auto builtin = dynamic_cast<BuiltinType *>(subexpr->getType());
    if (!builtin) throw TypeError{"Only Builtin type support UnaryExpr for now"};

    auto supported = builtin->getSupportedUnaryOps();
    if (!supported.contains(expr.getUnaryOp())) {
        throw TypeError{
            builtin->getTypeString() + " does not have operator " +
                Operator::getUnaryOperator(expr.getUnaryOp())
        };
    }

    auto resultType = supported[expr.getUnaryOp()];
    expr.setType(typeContext.getBuiltinType(resultType));

    return Generic<Expression *>::Create(&expr);
}

OpaqueType ExpressionAnalyzer::visit(BinaryExpr &expr) {
    auto lhsExpr = Generic<Expression *>::Get(expr.getLhs()->accept(this));
    auto rhsExpr = Generic<Expression *>::Get(expr.getRhs()->accept(this));
    auto lhs = dynamic_cast<BuiltinType *>(lhsExpr->getType());
    auto rhs = dynamic_cast<BuiltinType *>(rhsExpr->getType());
    if (lhs && rhs) {
        auto lhsSupported = lhs->getSupportedBinaryOps();
        auto rhsSupported = rhs->getSupportedBinaryOps();
        if (lhsSupported.contains(expr.getBinaryOp())) {
            auto targetType = typeContext.getBuiltinType(lhsSupported.at(expr.getBinaryOp()));
            try {
                expr.setRhs(insertImplicitCast(rhsExpr, targetType));
                expr.setType(targetType);
                return Generic<Expression *>::Create(&expr);
            } catch (TypeError &err) {}
        }
        if (rhsSupported.contains(expr.getBinaryOp())) {
            auto targetType = typeContext.getBuiltinType(rhsSupported.at(expr.getBinaryOp()));
            try {
                expr.setLhs(insertImplicitCast(lhsExpr, targetType));
                expr.setType(targetType);
                return Generic<Expression *>::Create(&expr);
            } catch (TypeError &err) {}
        }
    }
    throw TypeError{
        "No viable conversion for " + Operator::getBinaryOperator(expr.getBinaryOp()) +
            " with argument " + lhsExpr->getType()->getTypeString() + " and " + rhsExpr->getType()->getTypeString()
    };
}

OpaqueType ExpressionAnalyzer::visit(CastExpr &expr) {
    // no type check is required, programmer takes responsibility of the validity of the cast
    expr.setFrom(Generic<Expression *>::Get(expr.getFrom()->accept(this)));
    return Generic<Expression *>::Create(&expr);
}

OpaqueType ExpressionAnalyzer::visit(IndexExpr &expr) {
    expr.getBaseExpr()->accept(this);
    expr.setIndex(insertImplicitCast(
        Generic<Expression *>::Get(expr.getIndex()->accept(this)),
        typeContext.getBuiltinType(BuiltinType::Integer))
    );
    auto baseType = expr.getType();
    if (!baseType->isReferenceType()) throw TypeError{"Cannot index into " + baseType->getTypeString()};

    auto arrRefType = dynamic_cast<ReferenceType *>(baseType);
    if (auto array = dynamic_cast<ArrayType *>(arrRefType->getRefType())) {
        auto resultType = array->getElementType();
        expr.setType(resultType);
        return Generic<Expression *>::Create(&expr);
    }
    throw TypeError{"Cannot index into non array type " + baseType->getTypeString()};
}

OpaqueType ExpressionAnalyzer::visit(SelectorExpr &expr) {
    auto baseexpr = Generic<Expression *>::Get(expr.getBaseExpr()->accept(this));
    expr.setBaseExpr(baseexpr);
    if (!baseexpr->getType()->isReferenceType()) {
        throw TypeError{
            "Cannot access attribute " + expr.getSelector() +
                " of a non-reference type " + baseexpr->getType()->getTypeString()
        };
    }
    auto baseRefType = dynamic_cast<ReferenceType *>(baseexpr->getType());
    if (auto classType = dynamic_cast<ClassType *>(baseRefType->getRefType())) {
        auto member = classType->getMemberReference(expr.getSelector());
        expr.setType(member->getMemberAttrType());
        return Generic<Expression *>::Create(&expr);
    } else if (auto interfaceType = dynamic_cast<ClassType *>(baseRefType->getRefType())) {
        auto member = interfaceType->getMemberReference(expr.getSelector());
        expr.setType(member->getMemberAttrType());
        return Generic<Expression *>::Create(&expr);
    } else if (expr.getSelector() == "size") {
        auto arrayType = dynamic_cast<ArrayType *>(baseRefType->getRefType());
        if (!arrayType) throw TypeError{baseRefType->getRefType()->getTypeString() + " does not have attr size"};
        expr.setType(typeContext.getBuiltinType(BuiltinType::Integer));
        return Generic<Expression *>::Create(&expr);
    }
    throw TypeError{
        baseexpr->getType()->getTypeString() + " does not have attribute " + expr.getSelector()
    };
}

OpaqueType ExpressionAnalyzer::visit(ArgumentExpr &expr) {
    auto base = Generic<Expression *>::Get(expr.getBaseExpr()->accept(this));
    expr.setBaseExpr(base);
    // check for class instantiation
    if (auto classType = dynamic_cast<ClassType *>(base->getType())) {
        if (classType->isAbstract())
            throw TypeError{"Cannot instantiate abstract class " + classType->getTypeString()};
        // ignore constructor
        auto instanceType = typeContext.getReferenceType(classType, false);
        expr.setType(instanceType);
        return Generic<Expression *>::Create(&expr);
    }
    FunctionType *funcType = nullptr;
    // regular function calls
    if (auto regularFunc = dynamic_cast<FunctionType *>(base->getType())) {
        funcType = regularFunc;
    } else if (auto funcRef = dynamic_cast<ReferenceType *>(base->getType())) {
        // lambda ref calls
        if (auto baseFuncType = dynamic_cast<FunctionType *>(funcRef->getRefType())) {
            funcType = baseFuncType;
        }
    }
    if (funcType) {
        if (funcType->getParamTypes().size() != expr.getArguments().size()) {
            throw TypeError{
                "Expected " + std::to_string(funcType->getParamTypes().size()) +
                    " but received " + std::to_string(expr.getArguments().size()) + " instead"
            };
        }
        size_t index = 0;
        for (auto arg: expr.getArguments()) {
            auto given = Generic<Expression *>::Get(arg->accept(this));
            auto expected = insertImplicitCast(given, funcType->getParamTypes()[index]);
            expr.setArgument(expected, index);
            ++index;
        }
        return Generic<Expression *>::Create(&expr);
    }
    throw TypeError{"Unable to apply arguments to " + base->getType()->getTypeString()};
}

OpaqueType ExpressionAnalyzer::visit(ArrayLiteral &literal) {
    // todo: implement type hints
    return {};
}

OpaqueType ExpressionAnalyzer::visit(FunctionLiteral &literal) {
    parent.analyzeLambda(&literal);
    return Generic<Expression *>::Create(&literal);
}

OpaqueType ExpressionAnalyzer::visit(ImplicitCastExpr &expr) {
    return Generic<Expression *>::Create(&expr);
}

Expression *ExpressionAnalyzer::insertImplicitCast(Expression *expr, Type *targetType) {
    // todo: refactor this
    if (dynamic_cast<VoidType *>(expr->getType()))
        throw TypeError{"Cannot convert from void type"};

    auto fromType = expr->getType();
    if (fromType == targetType) return expr; // type identity expect ref type

    // implicit bool conversion
    if (targetType == typeContext.getBuiltinType(BuiltinType::Boolean)) {
        Operator::ImplicitConversion conversion;
        if (fromType == typeContext.getBuiltinType(BuiltinType::Character)) {
            conversion = Operator::ImplicitConversion::CharToBool;
        } else if (fromType == typeContext.getBuiltinType(BuiltinType::Integer)) {
            conversion = Operator::ImplicitConversion::IntToBool;
        } else if (fromType == typeContext.getBuiltinType(BuiltinType::Number)) {
            conversion = Operator::ImplicitConversion::NumToBool;
        } else if (fromType->isReferenceType()) {
            conversion = Operator::ImplicitConversion::RefToBool;
        } else {
            throw TypeError{"Unreachable: Cannot perform implicit bool conversion"}; // unreachable
        }
        auto cast = astContext.create<ImplicitCastExpr>(expr->location(), expr, conversion);
        cast->setType(targetType);
        return cast;
    }

    // builtin type promotion and widening
    Operator::ImplicitConversion conversion;
    if (fromType == typeContext.getBuiltinType(BuiltinType::Boolean)) {
        if (targetType == typeContext.getBuiltinType(BuiltinType::Character)) {
            conversion = Operator::ImplicitConversion::BoolToChar;
        } else if (targetType == typeContext.getBuiltinType(BuiltinType::Integer)) {
            conversion = Operator::ImplicitConversion::BoolToInt;
        } else if (targetType == typeContext.getBuiltinType(BuiltinType::Number)) {
            conversion = Operator::ImplicitConversion::BoolToNum;
        } else {
            throw TypeError{"Cannot promote BooleanType to " + targetType->getTypeString()};
        }
    } else if (fromType == typeContext.getBuiltinType(BuiltinType::Character)) {
        if (targetType == typeContext.getBuiltinType(BuiltinType::Integer)) {
            conversion = Operator::ImplicitConversion::CharToInt;
        } else if (targetType == typeContext.getBuiltinType(BuiltinType::Number)) {
            conversion = Operator::ImplicitConversion::CharToNum;
        } else {
            throw TypeError{"Cannot promote CharacterType to " + targetType->getTypeString()};
        }
    } else if (fromType == typeContext.getBuiltinType(BuiltinType::Integer)) {
        if (targetType == typeContext.getBuiltinType(BuiltinType::Number)) {
            conversion = Operator::ImplicitConversion::IntToNum;
        } else {
            throw TypeError{"Cannot promote IntegerType to " + targetType->getTypeString()};
        }
    } else if (fromType->isReferenceType() && targetType->isReferenceType()) {
        auto fromRefType = dynamic_cast<ReferenceType *>(fromType);
        auto targetRefType = dynamic_cast<ReferenceType *>(targetType);

        // nullptr conversion
        if (!fromRefType->getRefType()) {
            conversion = Operator::ImplicitConversion::NullToRef;
            if (!targetRefType->isNullable()) {
                throw TypeError{
                    "ImplicitCast from " + fromType->getTypeString() +
                        " drops null qualifier " + targetType->getTypeString()};
            }
            auto cast = astContext.create<ImplicitCastExpr>(expr->location(), expr, conversion);
            cast->setType(targetRefType);
        }

        // reference null cast
        if (fromRefType->getRefType() == targetRefType->getRefType()) {
            conversion = Operator::ImplicitConversion::RefNullable;
            if (!targetRefType->isNullable()) {
                throw TypeError{
                    "ImplicitCast from " + fromType->getTypeString() +
                        " drops null qualifier " + targetType->getTypeString()};
            }
            auto cast = astContext.create<ImplicitCastExpr>(expr->location(), expr, conversion);
            cast->setType(targetRefType);
            return cast;
        }

        // reference down cast
        if (fromRefType->getRefType()->isClassOrInterfaceType() &&
            targetRefType->getRefType()->isClassOrInterfaceType()) {
            conversion = Operator::ImplicitConversion::RefDownCast;
            auto from = dynamic_cast<CompositeType *>(fromRefType->getRefType());
            auto target = dynamic_cast<CompositeType *>(targetRefType->getRefType());

            // class downcast conversion
            if (from->isClassType() && target->isClassType()) {
                auto fromClass = dynamic_cast<ClassType *>(from);
                auto targetClass = dynamic_cast<ClassType *>(target);
                if (fromClass->isDerivedFrom(targetClass)) {
                    auto cast = astContext.create<ImplicitCastExpr>(expr->location(), expr, conversion);
                    cast->setType(targetRefType);
                    return cast;
                }
                throw TypeError{
                    fromClass->getTypeString() + " is not derived from " +
                        targetClass->getTypeString()
                };
            }
            // interface downcast conversion
            if (!from->isClassType() && !target->isClassType()) {
                auto fromInterface = dynamic_cast<InterfaceType *>(from);
                auto targetInterface = dynamic_cast<InterfaceType *>(target);
                if (fromInterface->isDerivedFrom(targetInterface)) {
                    auto cast = astContext.create<ImplicitCastExpr>(expr->location(), expr, conversion);
                    cast->setType(targetRefType);
                    return cast;
                }
                throw TypeError{
                    fromInterface->getTypeString() + " is not derived from " +
                        targetInterface->getTypeString()
                };
            }
            // class to interface conversion
            if (from->isClassType() && !target->isClassType()) {
                auto fromClass = dynamic_cast<ClassType *>(from);
                auto targetInterface = dynamic_cast<InterfaceType *>(target);
                if (fromClass->implements(targetInterface)) {
                    auto cast = astContext.create<ImplicitCastExpr>(expr->location(), expr, conversion);
                    cast->setType(targetRefType);
                    return cast;
                }
                throw TypeError{
                    fromClass->getTypeString() + " does not implement " +
                        targetInterface->getTypeString()
                };
            }
        }
    }

    throw TypeError{
        "No implicit conversion from " + fromType->getTypeString() +
            " to " + targetType->getTypeString()
    };
}

} // reflex
