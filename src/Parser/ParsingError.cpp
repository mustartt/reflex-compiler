//
// Created by henry on 2022-04-24.
//

#include "ParsingError.h"

#include <SourceManager.h>

namespace reflex {

void ParsingExpectedTokenError::printErrorMessage(std::ostream &os) const {
    const auto location = actualToken.getLocInfo();
    os << location->getLocationString() << ": ";
    os << "expected " << expectedType.getTypeString() << " ";
    os << "but got " << actualToken.getTokenType().getTypeString() << std::endl;
    location->printSourceRegion(os, true);
}

}
