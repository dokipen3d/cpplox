#pragma once
#include "TokenTypes.h"
#include <exception>

namespace cpplox {

class RuntimeError : std::runtime_error {
  public:
    RuntimeError(Token token, std::string message)
        : token(token), std::runtime_error(message) {
    }

  private:
    Token token;

    friend struct Error;
};

class ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace cpplox
