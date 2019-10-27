#include "Error.h"
#include <iostream>

namespace cpplox {
void Error::report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
              << "\n";
    hadError = true;
}

void Error::error(int line, const std::string& message) {
    report(line, "", message);
}

void Error::error(Token token, std::string message) {
    if (token.eTokenType == ETokenType::END_OF_FILE) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}
void Error::runtimeError(const RuntimeError& error) {
    std::cout << error.what() << "\n[line " << error.token.line << "]" << "\n";
    hadRuntimeError = true;
}

} // namespace cpplox
