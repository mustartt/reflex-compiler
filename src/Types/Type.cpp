//
// Created by henry on 2022-03-24.
//

#include "Type.h"

namespace reflex {

std::string getVisibilityString(Visibility visibility) {
    switch (visibility) {
        case Visibility::Public: return "public";
        case Visibility::Private: return "private";
        case Visibility::Protected: return "protected";
    }
}

void VoidType::printType(std::ostream &os) { os << "void"; }

void PrimType::printType(std::ostream &os) {
    switch (baseTyp) {
        case Integer: os << "int";
            break;
        case Number:os << "num";
            break;
        case Character:os << "char";
            break;
        case Boolean:os << "bool";
            break;
    }
}

void ArrType::printType(std::ostream &os) {
    os << "ArrayType: ";
    elementTyp->printType(os);
    os << "[]";
}

bool ArrType::operator==(const ArrType &rhs) const {
    return elementTyp == rhs.elementTyp;
}

void FuncType::printType(std::ostream &os) {
    os << "FunctionType: ";
    returnTyp->printType(os);
    os << "(";
    if (!paramTyp.empty()) {
        paramTyp[0]->printType(os);
        for (size_t i = 1; i < paramTyp.size(); ++i) {
            os << ",";
            paramTyp[i]->printType(os);
        }
    }
    os << ")";
}

bool FuncType::operator==(const FuncType &rhs) const {
    return returnTyp == rhs.returnTyp
        && paramTyp == rhs.paramTyp;
}

void MemberType::printType(std::ostream &os) {
    os << "    MemberType: " << getVisibilityString(visibility) << " ";
    memberTyp->printType(os);
}

bool MemberType::operator==(const MemberType &rhs) const {
    return instanceTyp == rhs.instanceTyp
        && visibility == rhs.visibility
        && memberTyp == rhs.memberTyp;
}

void InterfaceType::printType(std::ostream &os) {
    os << "InterfaceType: " << "(" << interfaces.size() << ")" << std::endl;
    for (auto &[name, overloads]: members) {
        os << "  " << name << ": " << std::endl;
        for (auto overload: overloads) {
            overload->printType(os);
        }
    }
}

bool InterfaceType::operator==(const InterfaceType &rhs) const {
    return interfaces == rhs.interfaces
        && members == rhs.members;
}

void ClassType::printType(std::ostream &os) {
    os << "ClassType: " << "(" << std::internal << std::hex << baseclass << ") -> "
       << interfaces.size() << std::endl;
    for (auto &[name, overloads]: members) {
        os << "  " << name << ": " << std::endl;
        for (auto overload: overloads) {
            overload->printType(os);
        }
    }
}

bool ClassType::operator==(const ClassType &rhs) const {
    return baseclass == rhs.baseclass
        && interfaces == rhs.interfaces
        && members == rhs.members;
}

}
