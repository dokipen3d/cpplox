#include "Expression.h"
#include <array>
#include <functional>
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
                 recursive_wrapper<Literal>, recursive_wrapper<Unary>>;

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



void parse(std::vector<Token>& tokens) {

    int current = 0;

    auto peek = [&]() -> Token { return tokens[current]; };

    auto isAtEnd = [&]() -> bool {
        return peek().eTokenType == ETokenType::END_OF_FILE;
    };

    auto previous = [&]() -> Token { return tokens[current - 1]; };

    auto advance = [&]() -> Token {
        if (!isAtEnd())
            current++;
        return previous();
    };

    auto check = [&](ETokenType type) -> bool {
        if (isAtEnd())
            return false;
        return peek().eTokenType == type;
    };

    auto match = [&](std::initializer_list<ETokenType> types) -> bool {
        for (const ETokenType& type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }

        return false;
    };

    auto consume = [&](ETokenType type, std::string message) -> Token {
        if (check(type))
            return advance();

        // throw error(peek(), message);
    };

    std::function<Expr()> primary;
    std::function<Expr()> expression;
    std::function<Expr()> unary;

    primary = [&]() -> Expr {
        if (match({ETokenType::FALSE}))
            return Literal(false);
        if (match({ETokenType::TRUE}))
            return Literal(true);
        if (match({ETokenType::NIL}))
            return Literal(nullptr);

        if (match({ETokenType::NUMBER, ETokenType::STRING})) {
            return Literal(previous().literal);
        }

        if (match({ETokenType::LEFT_PARENTHESIS})) {
            Expr expr = expression();
            consume(ETokenType::RIGHT_PARENTHESIS,
                    "Expect ')' after expression.");
            return Grouping(expr);
        }
    };

    unary = [&]() -> Expr {
        if (match({ETokenType::BANG, ETokenType::MINUS})) {
            Token _operator = previous();
            Expr right = unary();
            return Unary(_operator, right);
        }

        return primary();
    };

    auto multiplication = [&]() -> Expr {
        Expr expr = unary();

        while (match({ETokenType::SLASH, ETokenType::STAR})) {
            Token _operator = previous();
            Expr right = unary();
            expr = Binary(expr, _operator, right);
        }

        return expr;
    };

    auto addition = [&]() -> Expr {
        Expr expr = multiplication();

        while (match({ETokenType::MINUS, ETokenType::PLUS})) {
            Token _operator = previous();
            Expr right = multiplication();
            expr = Binary(expr, _operator, right);
        }

        return expr;
    };

    auto comparison = [&]() -> Expr {
        Expr expr = addition();

        while (match({ETokenType::GREATER, ETokenType::GREATER_EQUAL,
                      ETokenType::LESS, ETokenType::LESS_EQUAL})) {
            Token _operator = previous();
            Expr right = addition();
            expr = Binary(expr, _operator, right);
        }

        return expr;
    };

    auto equality = [&]() -> Expr {
        Expr expr = comparison();

        while (match({ETokenType::BANG_EQUAL, ETokenType::EQUAL_EQUAL})) {
            Token _operator = previous();
            Expr right = comparison();
            expr = Binary(expr, _operator, right);
        }

        return expr;
    };

    expression = [&]() -> Expr { return equality(); };

    // void visit2() {
    //     visitor v;
    //     // auto l1 = Literal{Object{"lit"}};
    //     // auto l2 = Literal{Object{"li2"}};
    //     // auto expr = Binary(l1, l2);
    //     // Expr expr = Binary(Literal(Object{"lit3"}),
    //     //                    Token{ETokenType::PLUS, "+", nullptr, 0},
    //     //                    Literal(Object{"lit4"}));

    //     Expr expr = Binary(Unary(Token(ETokenType::MINUS, "-", nullptr, 1),
    //                              Literal(Object{"124"})),
    //                        Token(ETokenType::STAR, "*", nullptr, 1),
    //                        Grouping(Literal(Object{"45.67"})));

    //     // std::visit(v, expr);
    //     v.print(expr);
    // }
}
} // namespace cpplox
