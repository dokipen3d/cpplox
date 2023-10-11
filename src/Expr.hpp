#pragma once

//#include "Object.h"

#include "Utilities.hpp"
#include "TokenTypes.h"

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
struct Increment;
struct Decrement;
struct Get;
struct Set;

struct ExprVoidType {};

struct Literal {
    Literal(Object val) : val(std::move(val)) {
    }
    Object val;
};


using ExprVariant =
    std::variant<void*, recursive_wrapper<Assign>, recursive_wrapper<Binary>,
                 recursive_wrapper<Grouping>, recursive_wrapper<Literal>, recursive_wrapper<Unary>,
                 recursive_wrapper<Variable>, recursive_wrapper<Logical>,
                 recursive_wrapper<Call>, recursive_wrapper<Increment>,
                 recursive_wrapper<Decrement>, recursive_wrapper<Get>,
                 recursive_wrapper<Set>>;

// helper functions to make variant comparable to nullptr
//////////////////////////////////////////////////////////////////////////

struct Expr final : ExprVariant {

    using ExprVariant::ExprVariant;
    using ExprVariant::operator=;

    //Expr(const Expr&) = default;

    // Expr(Expr&&) = default;
    // Expr& operator=(Expr& other) = default;
    // Expr& operator=(Expr&& other) = default;

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

    template <typename T> inline const T& get() const {
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

struct Increment {
    enum class Type { POSTFIX, PREFIX };

    Token op;
    Expr variable;
    Increment::Type type;
};

struct Decrement {
    enum class Type { POSTFIX, PREFIX };

    Token op;
    Expr variable;
    Increment::Type type;
};

struct Get {
    Expr object;
    Token name;
};

struct Set {
    Expr object;
    Token name;
    Expr value;
};

using LookupVariableVariant = std::variant<Variable, Assign>;

static_assert(std::is_move_constructible_v<Expr>,
              "Expr is not move contructible");
static_assert(std::is_move_assignable_v<Expr>, "Expr is not move contructible");


// template<>
// inline std::vector<std::vector<cpplox::Assign>> cpplox::recursive_wrapper<cpplox::Assign>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Binary>> cpplox::recursive_wrapper<cpplox::Binary>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Grouping>> cpplox::recursive_wrapper<cpplox::Grouping>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Unary>> cpplox::recursive_wrapper<cpplox::Unary>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Variable>> cpplox::recursive_wrapper<cpplox::Variable>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Logical>> cpplox::recursive_wrapper<cpplox::Logical>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Call>> cpplox::recursive_wrapper<cpplox::Call>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Increment>> cpplox::recursive_wrapper<cpplox::Increment>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Decrement>> cpplox::recursive_wrapper<cpplox::Decrement>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Get>> cpplox::recursive_wrapper<cpplox::Get>::t{};
// template<>
// inline std::vector<std::vector<cpplox::Set>> cpplox::recursive_wrapper<cpplox::Set>::t{};

static inline int createScriptStorageExpr(){

    cpplox::recursive_wrapper<cpplox::Assign>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Binary>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Grouping>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Unary>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Variable>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Logical>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Call>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Increment>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Decrement>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Get>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Set>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::Literal>::t.push_back({});


    return cpplox::recursive_wrapper<cpplox::Set>::t.size();
}

static inline void clearStorageExpr(){
    cpplox::recursive_wrapper<cpplox::Assign>::t.clear();
    cpplox::recursive_wrapper<cpplox::Binary>::t.clear();
    cpplox::recursive_wrapper<cpplox::Grouping>::t.clear();
    cpplox::recursive_wrapper<cpplox::Unary>::t.clear();
    cpplox::recursive_wrapper<cpplox::Variable>::t.clear();
    cpplox::recursive_wrapper<cpplox::Logical>::t.clear();
    cpplox::recursive_wrapper<cpplox::Call>::t.clear();
    cpplox::recursive_wrapper<cpplox::Increment>::t.clear();
    cpplox::recursive_wrapper<cpplox::Decrement>::t.clear();
    cpplox::recursive_wrapper<cpplox::Get>::t.clear();
    cpplox::recursive_wrapper<cpplox::Set>::t.clear();
    cpplox::recursive_wrapper<cpplox::Literal>::t.clear();

}

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
//         std::string hs = std::visit([](auto&& arg){ return arg.name.lexeme;},
//         luv); return std::hash<std::string>{}(hs);
//     }
// };
//} // namespace std