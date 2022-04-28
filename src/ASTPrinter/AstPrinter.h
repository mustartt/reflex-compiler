//
// Created by henry on 2022-03-21.
//

#ifndef REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_
#define REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_

#include <ASTVisitor.h>

#include <ostream>
#include <vector>
#include <stack>

namespace reflex {

class AstPrinter;
class Scope {
    AstPrinter &printer;
  public:
    explicit Scope(AstPrinter &printer, bool isLast = false);
    ~Scope();
};

class Type;
class AstPrinter : public ASTDeclVisitor,
                   public ASTStmtVisitor,
                   public ASTExprVisitor {
    friend class Scope;
  public:
    explicit AstPrinter(std::ostream &output);

    OpaqueType visit(CompilationUnit &CU) override;
    OpaqueType visit(ClassDecl &decl) override;
    OpaqueType visit(InterfaceDecl &decl) override;
    OpaqueType visit(VariableDecl &decl) override;
    OpaqueType visit(FieldDecl &decl) override;
    OpaqueType visit(ParamDecl &decl) override;
    OpaqueType visit(FunctionDecl &decl) override;
    OpaqueType visit(MethodDecl &decl) override;

    OpaqueType visit(BlockStmt &stmt) override;
    OpaqueType visit(ReturnStmt &stmt) override;
    OpaqueType visit(BreakStmt &stmt) override;
    OpaqueType visit(ContinueStmt &stmt) override;
    OpaqueType visit(IfStmt &stmt) override;
    OpaqueType visit(ForStmt &stmt) override;
    OpaqueType visit(WhileStmt &stmt) override;
    OpaqueType visit(EmptyStmt &stmt) override;
    OpaqueType visit(AssignmentStmt &stmt) override;
    OpaqueType visit(IncDecStmt &stmt) override;
    OpaqueType visit(ExpressionStmt &stmt) override;
    OpaqueType visit(DeclStmt &stmt) override;

    OpaqueType visit(DeclRefExpr &expr) override;
    OpaqueType visit(UnaryExpr &expr) override;
    OpaqueType visit(BinaryExpr &expr) override;
    OpaqueType visit(NewExpr &expr) override;
    OpaqueType visit(CastExpr &expr) override;
    OpaqueType visit(IndexExpr &expr) override;
    OpaqueType visit(SelectorExpr &expr) override;
    OpaqueType visit(ArgumentExpr &expr) override;

    OpaqueType visit(NumberLiteral &literal) override;
    OpaqueType visit(StringLiteral &literal) override;
    OpaqueType visit(BooleanLiteral &literal) override;
    OpaqueType visit(NullLiteral &literal) override;
    OpaqueType visit(ArrayLiteral &literal) override;
    OpaqueType visit(FunctionLiteral &literal) override;

  private:
    void printTreePrefix();
    void printNodePrefix(const std::string &message, bool end = true);
    std::string printAstType(Type *type);

    std::ostream &output;
    size_t depth;
    std::vector<bool> depthFlag;
    std::stack<bool> isLast;
};

}

#endif //REFLEX_SRC_AST_VISITOR_ASTPRINTER_H_
