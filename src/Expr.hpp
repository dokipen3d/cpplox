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
struct Call;

using Expr =
    std::variant<recursive_wrapper<Assign>, recursive_wrapper<Binary>,
                 recursive_wrapper<Grouping>, recursive_wrapper<Literal>,
                 recursive_wrapper<Unary>, recursive_wrapper<Variable>,
                 recursive_wrapper<Logical>, 
                 recursive_wrapper<Call>, void*>;

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
    explicit Literal(Object val) : val(std::move(val)) {
    }
    Object val;
};

struct Assign {
    Assign(Token name, Expr value)
        : name(std::move(name)), value(value) {
    }
    Token name;
    Expr value;
};

struct Grouping {
    explicit Grouping(Expr expr) : expr(expr) {
    }
    Expr expr;
};

struct Binary {
    Binary(Expr left, Token op, Expr right)
        : left{left}, right{right}, op(std::move(op)) {
    }

    Expr left;
    Expr right;
    Token op;
};

struct Unary {
    Unary(Token token, Expr expr)
        : token(std::move(token)), expr(expr) {
    }
    Token token;
    Expr expr;
};

struct Variable {
    explicit Variable(Token name) : name(std::move(name)) {
    }
    Token name;
};

struct Logical {
     Logical(Expr left, Token op, Expr right)
        : left{left}, right{right}, op(std::move(op)) {
    }

    Expr left;
    Expr right;
    Token op;
};


struct Call {
     Call(Expr callee, Token paren, std::vector<Expr> arguments)
        : callee{callee}, paren{paren}, arguments(std::move(arguments)) {
    }

    Expr callee;
    Token paren;
    std::vector<Expr> arguments;
};

static_assert(std::is_move_constructible_v<Expr>,
              "Expr is not move contructible");
static_assert(std::is_move_assignable_v<Expr>, "Expr is not move contructible");

} // namespace cpplox
