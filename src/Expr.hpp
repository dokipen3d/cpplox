#pragma once

//#include "Object.h"

#include "TokenTypes.h"
#include "Utilities.hpp"
#include <functional>
#include <iostream>
#include <variant>
//#include "thirdparty/mvariant.hpp"
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
struct ExprVoidType {};

using ExprVariant =
    std::variant<recursive_wrapper2<Assign>, recursive_wrapper2<Binary>,
                 recursive_wrapper2<Grouping>, recursive_wrapper2<Literal>,
                 recursive_wrapper2<Unary>, recursive_wrapper2<Variable>,
                 recursive_wrapper2<Logical>, recursive_wrapper2<Call>,
                 ExprVoidType*>;

// helper functions to make variant comparable to nullptr
//////////////////////////////////////////////////////////////////////////

struct Expr final : ExprVariant {

    using ExprVariant::ExprVariant;
    using ExprVariant::operator=;

    inline bool operator==(std::nullptr_t)
        const { // needs to be inline because its a free function that
                // it included in multiple translation units. needs to
                // be marked inline so linker knows its the same one
        return std::holds_alternative<ExprVoidType*>(*this);
    }

    inline bool operator!=(std::nullptr_t ptr) const {
        return !(*this == ptr);
    }

    template <typename T>
    bool is() const { // function needs to be const  to make it callable
        // from a const ref
        using actualTypeToGet =
            std::conditional_t<has_type_v<recursive_wrapper2<T>, ExprVariant>,
                               recursive_wrapper2<T>, T>;
        return std::holds_alternative<actualTypeToGet>(*this);
    }

    template <typename T>
    inline const T& expr_get() { // function needs to be const  to make it
                                 // callable from a const ref
        constexpr bool isRecursive =
            has_type<recursive_wrapper2<T>, ExprVariant>::value;

        return std::get<recursive_wrapper2<T>>(*this);
    }

    template <typename T> inline const T& get() {
        // constexpr bool isRecursive = ;
        using actualTypeToGet =
            std::conditional_t<has_type_v<recursive_wrapper2<T>, ExprVariant>,
                               recursive_wrapper2<T>, T>;
        return std::get<actualTypeToGet>(*this);
    }
};

/////////////////////////////////////////////////////////////////////
struct Literal {
    explicit Literal(Object val) : val(std::move(val)) {
    }
    Object val;
    // bool operator==(const Literal& other) const {
    //     return (val == other.val);
    // }
};

struct Assign {
    Assign(Token name, Expr value) : name(std::move(name)), value(value) {
    }
    Token name;
    Expr value;
    mutable int distance = -1;


    // bool operator==(const Assign& other) const {
    //     return ((name == other.name) && (value == other.value));
    // }
};

struct Grouping {
    explicit Grouping(Expr expr) : expr(expr) {
    }
    Expr expr;
    // bool operator==(const Grouping& other) const {
    //     return (expr == other.expr);
    // }
};

struct Binary {
    Binary(Expr left, Token op, Expr right)
        : left{left}, right{right}, op(std::move(op)) {
    }

    Expr left;
    Expr right;
    Token op;
    // bool operator==(const Binary& other) const {
    //     return ((left == other.left) && (right == other.right) &&
    //             (op == other.op));
    // }
};

struct Unary {
    Unary(Token token, Expr expr) : token(std::move(token)), expr(expr) {
    }
    Token token;
    Expr expr;

    // bool operator==(const Unary& other) const {
    //     return ((token == other.token) && (expr == other.expr));
    // }
};

struct Variable {
    explicit Variable(Token name) : name(std::move(name)) {
    }
    Token name;
    mutable int distance = -1;
    // bool operator==(const Variable& other) const {
    //     return (name == other.name);
    // }
};

struct Logical {
    Logical(Expr left, Token op, Expr right)
        : left{left}, right{right}, op(std::move(op)) {
    }

    Expr left;
    Expr right;
    Token op;
    // bool operator==(const Logical& other) const {
    //     return ((left == other.left) && (right == other.right) &&
    //             (op == other.op));
    // }
};

struct Call {
    Call(Expr callee, Token paren, std::vector<Expr> arguments)
        : callee{callee}, paren{paren}, arguments(std::move(arguments)) {
    }

    Expr callee;
    Token paren;
    std::vector<Expr> arguments;
    // bool operator==(const Call& other) const {
    //     return ((callee == other.callee) && (paren == other.paren) &&
    //             (arguments == other.arguments));
    // }
};

using LookupVariableVariant = std::variant<Variable, Assign>;

static_assert(std::is_move_constructible_v<Expr>,
              "Expr is not move contructible");
static_assert(std::is_move_assignable_v<Expr>, "Expr is not move contructible");

} // namespace cpplox

// these are needed for gcc to be able to visit exprs

template <>
struct std::variant_size<cpplox::Expr>
    : std::variant_size<cpplox::ExprVariant> {};

template <std::size_t I>
struct std::variant_alternative<I, cpplox::Expr>
    : std::variant_alternative<I, cpplox::ExprVariant> {};

// //custom specialization of std::hash can be injected in namespace std
// namespace std {
// template <> struct hash<cpplox::LookupVariableVariant> {
//     std::size_t operator()(cpplox::LookupVariableVariant const& luv) const
//         noexcept {
//         std::string hs = std::visit([](auto&& arg){ return arg.name.lexeme;}, luv);
//         return std::hash<std::string>{}(hs);
//     }
// };
//} // namespace std