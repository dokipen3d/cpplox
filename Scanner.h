#pragma once

#include <vector>
#include <string>
#include "TokenTypes.h"

inline bool hadError;

namespace cpplox {
    std::vector<Token> scanTokens(const std::string& source);

    void report(int line, const std::string& where, const std::string& message);
}

void error(int line, const std::string& message);
