#pragma once
#include "TokenTypes.h"
#include <exception>
#include <type_traits>

namespace cpplox {

class RuntimeError : std::runtime_error {
  public:
    RuntimeError(Token token, std::string message)
        : std::runtime_error(message), token(std::move(token)) {
    }

  private:
    Token token;

    friend struct Error;
};

class ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace cpplox
