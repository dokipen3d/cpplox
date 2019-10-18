#pragma once

#include <vector>
#include <string>
#include "tokenTypes.h"

inline bool hadError;

namespace cpplox {
    std::vector<Token> scanTokens(const std::string& code);

    void report(int line, const std::string& where, const std::string& message);
}

void error(int line, const std::string& message);
