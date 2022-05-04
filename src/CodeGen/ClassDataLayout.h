//
// Created by henry on 2022-05-03.
//

#ifndef REFLEX_SRC_CODEGEN_CLASSDATALAYOUT_H_
#define REFLEX_SRC_CODEGEN_CLASSDATALAYOUT_H_

#include <list>
#include <string>
#include <utility>
#include <ostream>
#include <map>

namespace reflex {

class Type;
class ClassType;
class FunctionType;
class InterfaceType;

class ClassDataLayout {
    using DataMember = std::pair<std::string, Type *>;
  public:
    explicit ClassDataLayout(ClassType *klass);

    long getFieldOffset(const std::string &field) const;
    void printClassLayout(std::ostream &os) const;

  private:
    std::list<DataMember> dataLayout;
    ClassType *reference;
};

class ClassVTableLayout {
  public:
    using VirtualMethod = std::pair<std::string, FunctionType *>;
    using VTableOffset = std::list<VirtualMethod>::const_iterator;

    explicit ClassVTableLayout(ClassType *klass);

    std::pair<long, VTableOffset> getMethodOffset(const std::string &method) const;
    const VirtualMethod &getMethodAtOffset(VTableOffset &offset) const;
    ClassType *getClassRef() const { return reference; }

    void printLayout(std::ostream &os) const;
  private:
    ClassType *reference;
    std::list<VirtualMethod> tableLayout;
};

class ClassITableLayout {
    using InterfaceMethod = std::tuple<std::string, ClassVTableLayout::VTableOffset, long>;
  public:
    ClassITableLayout(ClassVTableLayout &vtab, InterfaceType *impl, ClassITableLayout *next = nullptr);

    void printLayout(std::ostream &os) const;
  private:
    ClassType *baseclass;
    InterfaceType *interfaceImpl;
    std::vector<InterfaceMethod> tableLayout;
    ClassITableLayout *next;
};

class ClassRTTILayout {
  public:
    explicit ClassRTTILayout(ClassType *klass);

    void printLayout(std::ostream &os) const;

  private:
    ClassVTableLayout vtable;
    std::list<ClassITableLayout> itable;
    ClassType *reference;
};

} // reflex

#endif //REFLEX_SRC_CODEGEN_CLASSDATALAYOUT_H_
