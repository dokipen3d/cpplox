#pragma once

#include <variant>
#include <vector>
#include "TokenTypes.h"

namespace cpplox {


    void parse(std::vector<Token>& tokens);

    

//dvoid visit2();
struct visitor;

} // namespace cpplox
