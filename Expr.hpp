#pragma once
#ifndef EXPR
#define EXPR

#include "TokenTypes.h"
#include <variant>
#include <vector>

namespace cpplox {
// foward declares for recursive variant
struct Binary;
struct Grouping;
// struct Literal;
struct Unary;
struct NoOp {};

struct Literal {
    Literal(Object val) : val(val) {
    }
    Object val;
};

// helper class for holding forward declared types in variant
template <typename T> struct recursive_wrapper {
    recursive_wrapper(T t_) { // construct from an existing object
        t.emplace_back(std::move(t_));
    }
    operator const T&() { // cast back to wrapped type
        return t.front();
    }
    std::vector<T> t; // store the value
};

using Expr =
    std::variant<recursive_wrapper<Binary>, recursive_wrapper<Grouping>,
                 Literal, recursive_wrapper<Unary>, std::monostate, NoOp>;

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

} // namespace cpplox

#endif