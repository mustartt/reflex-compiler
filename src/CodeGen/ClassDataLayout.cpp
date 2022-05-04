//
// Created by henry on 2022-05-03.
//

#include <iomanip>
#include <unordered_set>

#include "ASTDeclaration.h"
#include "ClassDataLayout.h"

#include "Type.h"

namespace reflex {

ClassDataLayout::ClassDataLayout(ClassType *klass) : reference(klass) {
    do {
        auto insertPos = dataLayout.cbegin();
        for (auto &[field, memberType]: klass->getMembers()) {
            dataLayout.insert(insertPos, std::make_pair(field, memberType->getMemberAttrType()));
        }
        klass = klass->getBaseclass();
    } while (klass);
}

long ClassDataLayout::getFieldOffset(const std::string &field) const {
    auto iter = std::find_if(dataLayout.begin(), dataLayout.end(),
                             [&field](const auto &member) -> bool {
                               return field == member.first;
                             });
    if (iter == dataLayout.cend())
        throw TypeError{"Cannot find " + field + " in ClassLayout for " + reference->getDeclName()};
    return std::distance(dataLayout.begin(), iter);
}

void printDataLayoutHeader(std::ostream &os) {
    os << "Offset | Member Fields" << std::endl;
    os << std::string(50, '=') << std::endl;
}

void ClassDataLayout::printClassLayout(std::ostream &os) const {
    os << "DataLayout: " << reference->getDeclName() << std::endl;
    printDataLayoutHeader(os);

    auto sep = " | ";
    os << std::setw(4) << -1 << sep << "vtable*" << std::endl;
    for (auto &[field, type]: dataLayout) {
        os << std::setw(4) << getFieldOffset(field) << sep << field << std::endl;
    }
    os << std::endl;
}

ClassVTableLayout::ClassVTableLayout(ClassType *klass) : reference(klass) {
    do {
        auto insertPos = tableLayout.cbegin();
        for (auto &[method, memberType]: klass->getMethods()) {
            auto &methodname = method;
            auto iter = std::find_if(tableLayout.begin(), tableLayout.end(),
                                     [&](const auto &member) -> bool {
                                       return methodname == member.first;
                                     });
            if (iter != tableLayout.end()) continue;

            auto funcType = dynamic_cast<FunctionType *>(memberType->getMemberAttrType());
            tableLayout.insert(insertPos, std::make_pair(method, funcType));
        }
        klass = klass->getBaseclass();
    } while (klass);
}

std::pair<long, ClassVTableLayout::VTableOffset>
ClassVTableLayout::getMethodOffset(const std::string &method) const {
    auto iter = std::find_if(tableLayout.begin(), tableLayout.end(),
                             [&method](const auto &member) -> bool {
                               return method == member.first;
                             });
    if (iter == tableLayout.cend())
        throw TypeError{"Cannot find " + method + " in ClassVTableLayout for " + reference->getDeclName()};
    return {std::distance(tableLayout.begin(), iter), iter};
}

void printVTableHeader(std::ostream &os) {
    os << "Offset | VTable Methods" << std::endl;
    os << std::string(50, '=') << std::endl;
}

void ClassVTableLayout::printLayout(std::ostream &os) const {
    os << "VTable: " << reference->getDeclName() << std::endl;
    printVTableHeader(os);

    auto sep = " | ";
    for (auto &[method, type]: tableLayout) {
        auto [offset, _] = getMethodOffset(method);
        os << std::setw(4) << offset << sep << method << std::endl;
    }
    os << std::endl;
}

const ClassVTableLayout::VirtualMethod &
ClassVTableLayout::getMethodAtOffset(ClassVTableLayout::VTableOffset &offset) const {
    return *offset;
}

ClassITableLayout::ClassITableLayout(ClassVTableLayout &vtab, InterfaceType *impl, ClassITableLayout *next)
    : interfaceImpl(impl), baseclass(vtab.getClassRef()), next(next) {
    for (auto &[name, _]: impl->getInterfaceTraits()) {
        auto [index, offset] = vtab.getMethodOffset(name);
        tableLayout.emplace_back(name, offset, index);
    }
}

void printInterfaceHeader(std::ostream &os) {
    os << "Offset |     Interface Methods     | VTable Offset" << std::endl;
    os << std::string(50, '=') << std::endl;
}

void ClassITableLayout::printLayout(std::ostream &os) const {
    os << "ITable: " << interfaceImpl->getDeclName() << " <- " << baseclass->getDeclName();
    os << " " << std::hex << this << std::endl;

    printInterfaceHeader(os);

    auto sep = " | ";
    size_t tableOffset = 2;

    os << std::setw(6) << 0 << sep;
    os << std::left << std::setw(25) << "type_id " << std::right << sep;
    os << std::hex << std::setw(10) << interfaceImpl << std::endl;

    os << std::setw(6) << 1 << sep;
    os << std::left << std::setw(25) << "next " << std::right << sep;
    os << std::hex << std::setw(10) << next << std::endl;

    for (auto &[method, offset, vtabIndex]: tableLayout) {
        os << std::setw(6) << tableOffset << sep;
        os << std::left << std::setw(25) << offset->first << std::right << sep;
        os << std::setw(10) << vtabIndex << std::endl;
        ++tableOffset;
    }
    os << std::endl;
}

ClassRTTILayout::ClassRTTILayout(ClassType *klass) : reference(klass), vtable(klass) {
    std::vector<InterfaceType *> curr(klass->getInterfaces());
    std::vector<InterfaceType *> reverseVisitOrder;
    std::unordered_set<InterfaceType *> visited;

    while (!curr.empty()) {
        std::vector<InterfaceType *> next;
        for (auto i: curr) {
            if (!visited.contains(i)) {
                reverseVisitOrder.push_back(i);
            }
            visited.insert(i);
            for (auto inherited: i->getInterfaces()) {
                if (!visited.contains(inherited)) {
                    next.push_back(inherited);
                }
            }
        }
        curr = std::move(next);
    }

    if (!reverseVisitOrder.empty()) {
        itable.emplace_back(vtable, reverseVisitOrder[0]);
        for (size_t i = 1; i < reverseVisitOrder.size(); ++i) {
            itable.emplace_front(vtable, reverseVisitOrder[i], &itable.front());
        }
    }
}

void ClassRTTILayout::printLayout(std::ostream &os) const {
    vtable.printLayout(os);
    for (auto &i: itable) {
        i.printLayout(os);
    }
}

} // reflex
