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

// recursive wrapper with single vector for each type
template <typename T> struct recursive_wrapper {
    // construct from an existing object
    recursive_wrapper(T t_) {
        t.push_back(std::move(t_));
        index = t.size() - 1;
    }
    // cast back to wrapped type
    // operator const T &()  { return t.front(); }
    operator const T&() {
        return t[index];
    }

    // store the value
    static std::vector<T> t;
    int index;
    // std::basic_string<T> t;
};

template <typename T> inline std::vector<T> recursive_wrapper<T>::t;

using Expr =
    std::variant<recursive_wrapper<Binary>, recursive_wrapper<Grouping>,
                 Literal, recursive_wrapper<Unary>, std::monostate>;

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