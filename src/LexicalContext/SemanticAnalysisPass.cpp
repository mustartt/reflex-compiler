//
// Created by henry on 2022-04-30.
//

#include "SemanticAnalysisPass.h"

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

void SemanticAnalysisPass::analyzeFunction(FunctionDecl *decl) {
    if (symbolTables.empty()) {
        symbolTables.push(std::make_unique<SymbolTable>(nullptr));
    } else {
        symbolTables.push(std::make_unique<SymbolTable>(symbolTables.top().get()));
    }

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
    symbolTables.pop();
}

void SemanticAnalysisPass::analyzeMethod(MethodDecl *decl) {
    symbolTables.push(std::make_unique<SymbolTable>(nullptr));

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

    symbolTables.pop();
}

void SemanticAnalysisPass::analyzeLambda(FunctionLiteral *literal) {
    symbolTables.push(std::make_unique<SymbolTable>(symbolTables.top().get()));

    symbolTables.pop();
}

OpaqueType SemanticAnalysisPass::visit(BlockStmt &stmt) {
    symbolTables.push(std::make_unique<SymbolTable>(symbolTables.top().get()));
    lexicalScopes.push(stmt.getScope()->getChild());

    for (auto s: stmt.getStmts()) {
        s->accept(this);
    }

    lexicalScopes.pop();
    symbolTables.pop();
    return {};
}

OpaqueType SemanticAnalysisPass::visit(DeclStmt &stmt) {
    auto decl = stmt.getDecl();
    if (auto var = dynamic_cast<VariableDecl *>(decl)) {
        auto type = typeParser.parseReferenceTypeExpr(var->getTypeDecl(), lexicalScopes.top());
        var->setType(type);
        decl->setType(type);
        symbolTables.top()->add(decl);
        // todo: visit initializer
    }
    // todo: visit nested interface and class methods
    return {};
}

OpaqueType SemanticAnalysisPass::visit(BreakStmt &stmt) {
    if (!inloop) throw AnalysisError{"BreakStmt is not contained in a loop"};
    return {};
}

OpaqueType SemanticAnalysisPass::visit(ContinueStmt &stmt) {
    if (!inloop) throw AnalysisError{"ContinueStmt is not contained in a loop"};
    return {};
}

OpaqueType ExprAnalysisPass::visit(DeclRefExpr &expr) {
    auto name = expr.getReferenceName();
    try { // resolve in local scope
        auto decl = table->find(name);
        expr.setType(decl->getType());
        expr.setDecl(decl);
    } catch (ReferenceError &_) {
        try { // resolve in lexical scope
            auto member = lexicalscope->resolve(name);
            expr.setType(member->getMemberType());
            expr.setDecl(dynamic_cast<Declaration *>(member->getParent()->getNodeDecl()));
        } catch (LexicalError &err) {
            throw ReferenceError{err.what()};
        }
    }
    return Generic<Expression *>::Create(&expr);
}

OpaqueType ExprAnalysisPass::visit(NewExpr &expr) {
    return Generic<Expression *>::Create(&expr);
}

OpaqueType ExprAnalysisPass::visit(NumberLiteral &literal) {
    if (static_cast<int>(literal.getValue()) == literal.getValue()) {
        literal.setType(typeContext.getBuiltinType(BuiltinType::Integer));
    } else {
        literal.setType(typeContext.getBuiltinType(BuiltinType::Number));
    }
    return Generic<Expression *>::Create(&literal);
}

OpaqueType ExprAnalysisPass::visit(StringLiteral &literal) {
    return {}; // todo: implement string
}

OpaqueType ExprAnalysisPass::visit(BooleanLiteral &literal) {
    literal.setType(typeContext.getBuiltinType(BuiltinType::Boolean));
    return Generic<Expression *>::Create(&literal);
}

OpaqueType ExprAnalysisPass::visit(NullLiteral &literal) {
    literal.setType(typeContext.getReferenceType(nullptr));
    return Generic<Expression *>::Create(&literal);
}

Expression *ExprAnalysisPass::insertImplicitCast(Expression *expr, Type *targetType) {
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
