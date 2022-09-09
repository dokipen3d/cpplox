#pragma once

#include "ExceptionError.h"
#include "Expr.hpp"
#include "Statement.hpp"
#include "TokenTypes.h"
#include <vector>

namespace cpplox {

class Parser {
  public:
    size_t current = 0;
    Parser(std::vector<Token>& tokens);
    void print(const Expr& expr);
    void synchronize();

    bool isAtEnd();
    bool check(ETokenType type);
    bool match(std::initializer_list<ETokenType> types);

    // clang-format off
    auto parse() -> /*------------------------------------------------------->*/std::vector<Statement>;

    auto peek() -> /*-------------------------------------------------------->*/Token;
    auto previous() -> /*---------------------------------------------------->*/Token;
    auto advance() -> /*----------------------------------------------------->*/Token;
    auto consume(ETokenType type, const std::string& message) -> /*--------->*/ Token;

    auto function(std::string kind) -> /*------------------------------------>*/Statement;

    auto declaration() -> /*------------------------------------------------->*/Statement;
    auto varDeclaration() -> /*---------------------------------------------->*/Statement;
    auto classDeclaration() -> /*-------------------------------------------->*/Statement;

    auto statement() -> /*--------------------------------------------------->*/Statement;
    auto whileStatement() -> /*---------------------------------------------->*/Statement;
    auto returnStatement() -> /*--------------------------------------------->*/Statement;

    auto ifStatement() -> /*------------------------------------------------->*/Statement;
    auto forStatement() -> /*------------------------------------------------>*/Statement;


    auto printStatement() -> /*---------------------------------------------->*/Statement;
    auto expressionStatement() -> /*----------------------------------------->*/Statement;
    auto block() -> /*------------------------------------------------------->*/std::vector<Statement>;


    auto error(Token token, std::string message) -> /*----------------------->*/ParseError;

    // clang-format on
    Expr parseExpression();
    Expr primary();
    Expr unary();
    Expr multiplication();
    Expr addition();
    Expr comparison();
    Expr equality();
    Expr expression();
    Expr assignment();
    Expr logicalOr();
    Expr logicalAnd();
    Expr call();
    Expr finishCall(Expr callee);
    Expr prefix();
    Expr postfix();

    std::vector<Token>& tokens;

    //for pre hashing
    tsl::robin_map<std::string, Object> map;
};

struct visitor;

} // namespace cpplox
