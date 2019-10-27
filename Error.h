#pragma once

#include "RuntimeError.h"
#include "TokenTypes.h"
#include <string>

inline bool hadError = false;
inline bool hadRuntimeError = false;

namespace cpplox {
struct Error {
    static void report(int line, const std::string& where, const std::string& message);
    static void error(int line, const std::string& message);
    static void error(Token token, std::string message);
    static void runtimeError(const RuntimeError& error);
};
} // namespace cpplox
