#pragma once

#include "TokenTypes.h"
#include <string>
#include <vector>

namespace cpplox {
std::vector<Token> scanTokens(const std::string& source);
}
