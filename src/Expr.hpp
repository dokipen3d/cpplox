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
struct Logical;
struct Call;
struct ExprVoidType {};

struct Literal {
    Literal(Object val) : val(std::move(val)) {
    }
    Object val;
   
};

using ExprVariant =
    std::variant<void*, recursive_wrapper<Assign>, recursive_wrapper<Binary>,
                 recursive_wrapper<Grouping>, Literal,
                 recursive_wrapper<Unary>, recursive_wrapper<Variable>,
                 recursive_wrapper<Logical>, recursive_wrapper<Call>
                 >;

// helper functions to make variant comparable to nullptr
//////////////////////////////////////////////////////////////////////////

struct Expr final : ExprVariant {

    using ExprVariant::ExprVariant;
    using ExprVariant::operator=;

    inline bool operator==(std::nullptr_t)
        const { // needs to be inline because its a free function that
                // it included in multiple translation units. needs to
                // be marked inline so linker knows its the same one
        return std::holds_alternative<void*>(*this);
    }

    inline bool operator!=(std::nullptr_t ptr) const {
        return !(*this == ptr);
    }

    template <typename T>
    bool is() const { // function needs to be const  to make it callable
        // from a const ref
        using actualTypeToGet =
            std::conditional_t<has_type_v<recursive_wrapper<T>, ExprVariant>,
                               recursive_wrapper<T>, T>;
        return std::holds_alternative<actualTypeToGet>(*this);
    }

    template <typename T>
    inline const T& expr_get() { // function needs to be const  to make it
                                 // callable from a const ref
        constexpr bool isRecursive =
            has_type<recursive_wrapper<T>, ExprVariant>::value;

        return std::get<recursive_wrapper<T>>(*this);
    }

    template <typename T> inline const T& get() const  {
        // constexpr bool isRecursive = ;
        using actualTypeToGet =
            std::conditional_t<has_type_v<recursive_wrapper<T>, ExprVariant>,
                               recursive_wrapper<T>, T>;
        return std::get<actualTypeToGet>(*this);
    }

        template <typename T> inline T& get() {
        // constexpr bool isRecursive = ;
        using actualTypeToGet =
            std::conditional_t<has_type_v<recursive_wrapper<T>, ExprVariant>,
                               recursive_wrapper<T>, T>;
        return std::get<actualTypeToGet>(*this);
    }
};

/////////////////////////////////////////////////////////////////////


struct Assign {
    Assign(Token name, Expr value) : name(std::move(name)), value(value) {
      
    }
    Token name;
    Expr value;
    mutable int distance = -1;
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
    Token op;
    Expr left;
    Expr right;
};

struct Unary {
    Unary(Token token, Expr expr) : token(std::move(token)), expr(expr) {
    }
    Token token;
    Expr expr;
};

struct Variable {
    explicit Variable(Token name) : name(std::move(name)) {
        
    }
    Token name;
    mutable int distance = -1;
};

struct Logical {
    Logical(Expr left, Token op, Expr right)
        : left{left}, right{right}, op(std::move(op)) {
    }
    Token op;
    Expr left;
    Expr right;
};

struct Call {
    Call(Expr callee, Token paren, std::vector<Expr> arguments)
        : callee{callee}, paren{paren}, arguments(std::move(arguments)) {
    }
    Token paren;
    std::vector<Expr> arguments;
    Expr callee;
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