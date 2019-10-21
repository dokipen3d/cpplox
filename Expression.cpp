#include "TokenTypes.h"
#include <array>
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

namespace cpplox {

// foward declares for recursive variant
struct Binary;
struct Grouping;
struct Literal;
struct Unary;

// helper class for holding forward declared types in variant
template <typename T> struct recursive_wrapper {
    // construct from an existing object
    recursive_wrapper(T t_) {
        t.emplace_back(std::move(t_));
    }
    // cast back to wrapped type
    operator const T&() {
        return t.front();
    }
    // store the value
    std::vector<T> t;
};

struct Literal {
    Literal(Object val) : val(val) {
    }
    Object val;
};

using Expr =
    std::variant<recursive_wrapper<Binary>, recursive_wrapper<Grouping>,
                 Literal, recursive_wrapper<Unary>>;
/// recursive_wrapper<Unary>, recursive_wrapper<Grouping>,
struct Binary {

    Binary(Expr A, Token op, Expr B)
        : expressions{std::move(A), std::move(B)}, op(std::move(op)) {
    }

    std::array<Expr, 2> expressions;
    Token op;
};

struct Grouping {
    Grouping(Expr expr) : expr(std::move(expr)) {
    }
    Expr expr;
};

struct Unary {
    Unary(Token token, Expr expr)
        : token(std::move(token)), expr(std::move(expr)) {
    }
    Token token;
    Expr expr;
};

struct visitor {

    std::stringstream ast;
    void print(Expr expression) {
        std::visit(*this, static_cast<Expr>(expression));
        std::cout << ast.str();
    }

    void parenthesize(const std::string name,
                      const std::initializer_list<Expr> exprs) {
        ast << "(" << name;
        for (const auto& e : exprs) {
            ast << " ";
            std::visit(*this, static_cast<Expr>(e));
        }
        ast << ")";
    }

    void operator()(const Binary& binary) {
        parenthesize(binary.op.lexeme,
                     {binary.expressions[0], binary.expressions[1]});
        //}
    }
    void operator()(const Literal& literal) {
        if (literal.val.index() == 0) {
            ast << "nil";
            return;
        } else {
            std::visit(
                [&](auto&& arg) {
                    // not a void* so can print
                    ast << arg;
                },
                literal.val);
        }
    }

    void operator()(const Grouping& grouping) {
        parenthesize("group", {grouping.expr});
    }
    void operator()(const Unary& unary) {
        parenthesize(unary.token.lexeme, {unary.expr});

    }
};

void visit2() {
    visitor v;
    // auto l1 = Literal{Object{"lit"}};
    // auto l2 = Literal{Object{"li2"}};
    // auto expr = Binary(l1, l2);
    // Expr expr = Binary(Literal(Object{"lit3"}),
    //                    Token{ETokenType::PLUS, "+", nullptr, 0},
    //                    Literal(Object{"lit4"}));

    Expr expr = Binary(
        Unary(Token(ETokenType::MINUS, "-", nullptr, 1), Literal(Object{"124"})),
        Token(ETokenType::STAR, "*", nullptr, 1), Grouping(Literal(Object{"45.67"})));

    // std::visit(v, expr);
    v.print(expr);
}

} // namespace cpplox
