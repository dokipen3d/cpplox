#pragma once 

#include "TokenTypes.h"
#include <string>

inline bool hadError = false;

namespace cpplox {
struct Error {
    static void report(int line, const std::string& where, const std::string& message);
    static void error(int line, const std::string& message);
};

} //end namespace lox