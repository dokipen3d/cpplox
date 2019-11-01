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
struct Logical;

using Expr =
    std::variant<recursive_wrapper<Assign>, recursive_wrapper<Binary>,
                 recursive_wrapper<Grouping>, recursive_wrapper<Literal>,
                 recursive_wrapper<Unary>, recursive_wrapper<Variable>,
                 recursive_wrapper<Logical>, void*>;

// helper functions to make variant comparable to nullptr
//////////////////////////////////////////////////////////////////////////
inline bool operator==(
    const Expr& other,
    std::nullptr_t) { // needs to be inline because its a free function that
                      // it included in multiple translation units. needs to
                      // be marked inline so linker knows its the same one
    return std::holds_alternative<void*>(other);
}

inline bool operator!=(const Expr& other, std::nullptr_t ptr) {
    return !(other == ptr);
}

template <typename T>
bool is(const Expr& expr) { // function needs to be const  to make it callable
                            // from a const ref
    return std::holds_alternative<recursive_wrapper<T>>(expr);
}

template <typename T>
inline const T&
expr_get(const Expr& expr) { // function needs to be const  to make it callable
                             // from a const ref
    return std::get<recursive_wrapper<T>>(expr);
}

/////////////////////////////////////////////////////////////////////
struct Literal {
    explicit Literal(Object val) : val(val) {
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
    Binary(Expr left, Token op, Expr right)
        : left{std::move(left)}, right{std::move(right)}, op(std::move(op)) {
    }

    Expr left;
    Expr right;
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

struct Logical {
     Logical(Expr left, Token op, Expr B)
        : left{std::move(left)}, right{std::move(right)}, op(std::move(op)) {
    }

    Expr left;
    Expr right;
    Token op;
};

static_assert(std::is_move_constructible_v<Expr>,
              "Expr is not move contructible");
static_assert(std::is_move_assignable_v<Expr>, "Expr is not move contructible");

} // namespace cpplox
