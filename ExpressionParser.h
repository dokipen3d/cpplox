#pragma once

#include "ExceptionError.h"
#include "Expr.hpp"
#include "Statement.hpp"
#include "TokenTypes.h"
#include <vector>

namespace cpplox {

class Parser {
  public:
    int current = 0;
    Parser(std::vector<Token>& tokens);
    void print(const Expr& expr);
    void synchronize();

    bool isAtEnd();
    bool check(ETokenType type);
    bool match(std::initializer_list<ETokenType> types);

    auto parse()  -> std::vector<Statement>;

    auto peek() -> Token;
    auto previous() -> Token;
    auto advance() -> Token;
    auto consume(ETokenType type, const std::string& message) -> Token;

    auto statement() -> Statement;
    auto printStatement() -> Statement;
    auto expressionStatement() -> Statement;
    auto error(Token token, std::string message) -> ParseError;

    Expr parseExpression();
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
