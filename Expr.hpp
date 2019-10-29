#pragma once

#include "Utilities.hpp"
#include "TokenTypes.h"
#include <variant>
#include <vector>

namespace cpplox {
// foward declares for recursive variant
struct Binary;
struct Grouping;
struct Variable;
struct Unary;
struct NoOp {};

struct Literal {
    Literal(Object val) : val(val) {
    }
    Object val;
};

using Expr =
    std::variant<recursive_wrapper<Binary>, recursive_wrapper<Grouping>,
                 Literal, recursive_wrapper<Unary>, recursive_wrapper<Variable>, std::monostate, NoOp>;

struct Grouping {
    //, NoOp dummy
    explicit Grouping(Expr expr) : expr(std::move(expr)) {
    }
    // Token t;
    Expr expr;
    // int t;
};

struct Binary {
    Binary(Expr A, Token op, Expr B)
        : expressionA{std::move(A)}, expressionB{std::move(B)},
          op(std::move(op)) {
    }

    Expr expressionA;
    Expr expressionB;

    Token op;
};

struct Unary {
    Unary(Token token, Expr expr)
        : token(std::move(token)), expr(std::move(expr)) {
    }
    Token token;
    Expr expr;
};

struct Variable {
    Variable(Token name)
        : name(std::move(name)) {
    }
    Token name;
};



} // namespace cpplox

