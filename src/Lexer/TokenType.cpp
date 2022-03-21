//
// Created by henry on 2022-03-21.
//

#include "TokenType.h"

namespace reflex {

std::string TokenType::getTypeString() const {
    switch (value) {
        case EndOfFile: return "EOF";
        case WhiteSpace: return "WS";
        case NameSeparator: return "NAME_SEPARATOR";
        case Colon:return "COLON";
        case SemiColon:return "SEMICOLON";
        case Comma:return "COMMA";
        case Period:return "PERIOD";
        case Complement:return "COMPLEMENT";
        case Compare:return "COMPARE";
        case CompareNot:return "COMPARE_NOT";
        case CompareLessEqual:return "COMPARE_LESS_EQUAL";
        case CompareGreaterEqual:return "COMPARE_GREATER_EQUAL";
        case PostInc:return "POSTFIX_INC";
        case PostDec:return "POSTFIX_DEC";
        case LAngleBracket:return "LEFT_ANGLE_BRACKET";
        case RAngleBracket:return "RIGHT_ANGLE_BRACKET";
        case ReturnArrow:return "RETURN_ARROW";
        case AssignAdd:return "ASSIGN_ADD";
        case AssignSub:return "ASSIGN_SUB";
        case Add:return "ADD";
        case Sub:return "SUB";
        case Star:return "STAR";
        case Div:return "DIV";
        case Mod:return "MOD";
        case LogicalOr:return "LOGICAL_OR";
        case LogicalAnd:return "LOGICAL_AND";
        case LogicalNot:return "LOGICAL_NOT";
        case Assign:return "ASSIGN";
        case LBracket:return "LEFT_BRACKET";
        case RBracket:return "RIGHT_BRACKET";
        case LParen:return "LEFT_PAREN";
        case RParen:return "RIGHT_PAREN";
        case LBrace:return "LEFT_BRACE";
        case RBrace:return "RIGHT_BRACE";
        case Annotation:return "ANNOTATION";
        case SingleComment:return "SINGLE_COMMENT";
        case MultiComment:return "MULTI_COMMENT";
        case Identifier:return "IDENTIFIER";
        case NumberLiteral:return "NUMBER_LIT";
        case StringLiteral:return "STRING_LIT";
        case Const:return "CONST";
        case Var:return "VAR";
        case Func:return "FUNC";
        case Return:return "RETURN";
        case BoolLiteral:return "BOOL_LIT";
        case NullLiteral:return "NULL_LIT";
        case Class:return "CLASS";
        case New:return "NEW";
        case Interface:return "INTERFACE";
        case Extends:return "EXTENDS";
        case Implements:return "IMPLEMENTS";
        case Or:return "OR";
        case And:return "AND";
        case If:return "IF";
        case Else:return "ELSE";
        case For:return "FOR";
        case While:return "WHILE";
        case Cast:return "CAST";
        case Try:return "Try";
        case In:return "IN";
        case Catch:return "CATCH";
        case Break:return "BREAK";
        case Continue:return "CONTINUE";
    }
}

TokenType::Value TokenType::getValue() const {
    return value;
}

}
