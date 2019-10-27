#pragma once

#include "TokenTypes.h"
#include <vector>
#include "Expr.hpp"

namespace cpplox {

class Parser {
  public:
    int current = 0;
    Parser(std::vector<Token>& tokens);
    void print(const Expr& expr);

    Expr parse();

    Token peek();

    Token previous();

    bool isAtEnd();

    Token advance();

    bool check(ETokenType type);

    bool match(std::initializer_list<ETokenType> types);

    void synchronize();

    Token consume(ETokenType type, const std::string& message);

    Expr primary();

    Expr unary();
    Expr multiplication();
    Expr addition();
    Expr comparison();
    Expr equality();
    Expr expression();
    std::vector<Token>& tokens;
};

struct visitor;

} // namespace cpplox
