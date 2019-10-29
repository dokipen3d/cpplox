#pragma once

#include "TokenTypes.h"
#include "Utilities.hpp"
#include <variant>
#include <vector>

namespace cpplox {
// foward declares for recursive variant
struct Assign;
struct Binary;
struct Grouping;
struct Variable;
struct Unary;

struct Literal;

using Expr =
    std::variant<recursive_wrapper<Assign>, recursive_wrapper<Binary>,
                 recursive_wrapper<Grouping>, recursive_wrapper<Literal>,
                 recursive_wrapper<Unary>, recursive_wrapper<Variable>, void*>;

// helper functions to make variant comparable to nullptr
//////////////////////////////////////////////////////////////////////////
const inline bool operator==(
    const Expr& other,
    std::nullptr_t ptr) { // needs to be inline because its a free function that
                          // it included in multiple translation units. needs to
                          // be marked inline so linker knows its the same one
    return std::holds_alternative<void*>(other);
}

const inline bool operator!=(const Expr& other, std::nullptr_t ptr) {
    return !(other == ptr);
}
/////////////////////////////////////////////////////////////////////
struct Literal {
    Literal(Object val) : val(val) {
    }
    Object val;
};

struct Assign {
    Assign(Token name, Expr value)
        : name(std::move(name)), value(std::move(value)) {
    }
    Token name;
    Expr value;
};

struct Grouping {
    explicit Grouping(Expr expr) : expr(std::move(expr)) {
    }
    Expr expr;
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
    Variable(Token name) : name(std::move(name)) {
    }
    Token name;
};

static_assert(std::is_move_constructible_v<Expr>, "Expr is not move contructible");
static_assert(std::is_move_assignable_v<Expr>, "Expr is not move contructible");

} // namespace cpplox
