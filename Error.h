#pragma once

#include "TokenTypes.h"
#include <iostream>
#include <string>

inline bool hadError;

namespace cpplox {
namespace Error {
void report(int line, const std::string& where, const std::string& message);
void error(int line, const std::string& message);
void error(Token token, std::string message);
} // namespace Error
} // namespace cpplox
