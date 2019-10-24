#include "Error.h"

namespace cpplox {
namespace Error {
void report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
              << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}

void error(Token token, std::string message) {
    if (token.eTokenType == ETokenType::END_OF_FILE) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}
} // namespace Error
} // namespace cpplox