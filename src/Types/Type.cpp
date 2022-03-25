//
// Created by henry on 2022-03-24.
//

#include <sstream>
#include "Type.h"

namespace reflex {

std::string getVisibilityString(Visibility visibility) {
    switch (visibility) {
        case Visibility::Public: return "public";
        case Visibility::Private: return "private";
        case Visibility::Protected: return "protected";
    }
}

Visibility getVisibilityFromString(const std::string &ident) {
    if (ident == "public") return Visibility::Public;
    if (ident == "private") return Visibility::Private;
    if (ident == "protected") return Visibility::Protected;
    throw std::runtime_error("Unknown access modifier " + ident);
}

std::string VoidType::getTypeString() {
    return "void";
}

std::string PrimType::getTypeString() {
    switch (baseTyp) {
        case Integer: return "int";
        case Number: return "num";
        case Character: return "char";
        case Boolean: return "bool";
        case Null: return "null";
    }
}

bool ArrayType::operator==(const ArrayType &rhs) const {
    return elementTyp == rhs.elementTyp;
}

std::string ArrayType::getTypeString() {
    return elementTyp->getTypeString() + " []";
}

std::string FunctionType::getTypeString() {
    std::stringstream ss;
    ss << "(";
    if (!paramTyp.empty()) {
        ss << paramTyp[0]->getTypeString();
        for (size_t i = 1; i < paramTyp.size(); ++i) {
            ss << "," << paramTyp[i]->getTypeString();
        }
    }
    ss << ")->" << returnTyp->getTypeString();
    return ss.str();
}

bool FunctionType::operator==(const FunctionType &rhs) const {
    return returnTyp == rhs.returnTyp
        && paramTyp == rhs.paramTyp;
}

std::string MemberType::getTypeString() {
    return getVisibilityString(visibility) + " "
        + memberTyp->getTypeString();
}

bool MemberType::operator==(const MemberType &rhs) const {
    return instanceTyp == rhs.instanceTyp
        && visibility == rhs.visibility
        && memberTyp == rhs.memberTyp;
}

std::string InterfaceType::getTypeString() {
    return "interface " + interfacename;
}

bool InterfaceType::operator==(const InterfaceType &rhs) const {
    return interfaces == rhs.interfaces
        && members == rhs.members;
}

std::string ClassType::getTypeString() {
    return "class " + classname;
}

bool ClassType::operator==(const ClassType &rhs) const {
    return baseclass == rhs.baseclass
        && interfaces == rhs.interfaces
        && members == rhs.members;
}

}
