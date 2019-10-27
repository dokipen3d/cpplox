#include "ExpressionParser.h"
#include "Error.h"
#include <array>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

namespace cpplox {

struct visitor {

    std::stringstream ast;
    void print(Expr expression) {
        std::visit(*this, static_cast<Expr>(expression));
        std::cout << ast.str();
    }

    void parenthesize(const std::string& name, const Expr exprA,
                      const Expr exprB) {
        ast << "(" << name;

        ast << " ";
        std::visit(*this, static_cast<Expr>(exprA));
        ast << " ";
        std::visit(*this, static_cast<Expr>(exprB));

        ast << ")";
    }

    void parenthesize(const std::string& name, const Expr expr) {
        parenthesize(name, expr, std::monostate{});
    }

    void operator()(const Binary& binary) {
        parenthesize(binary.op.lexeme, binary.expressionA, binary.expressionB);
        //}
    }
    void operator()(const Literal& literal) {
        if (literal.val == nullptr) {
            ast << "nil";
        } else {
            std::visit(
                [&](auto&& arg) {
                    // not a void* so can print
                    ast << arg;
                },
                static_cast<variantObject>(literal.val));
        }
    }

    void operator()(const Grouping& grouping) {
        parenthesize("group", grouping.expr);
    }
    void operator()(const Unary& unary) {
        parenthesize(unary.token.lexeme, unary.expr);
    }
    void operator()(const std::monostate neverCalled) {
    }
};

Parser::Parser(std::vector<Token>& tokens) : tokens(tokens) {
}

auto Parser::print(const Expr& expr) -> void {
    visitor v;
    v.print(expr);
}

auto Parser::parse() -> Expr {
    try {
        Expr expr = expression();
        // extra check to find if there is a trailing token that didnt get
        // parsed
         if (tokens[current].eTokenType != ETokenType::END_OF_FILE) {
             Error::error(peek(), "Expect expression. got " + tokens[current].toString());
             throw std::runtime_error("Expect expression.");
         }
        return expr;
    } catch (const std::runtime_error& error) {
        std::cout << "caught!\n";
        return std::monostate{};
    }
}

auto Parser::peek() -> Token {
    return tokens[current];
};

auto Parser::previous() -> Token {
    return tokens[current - 1];
};

auto Parser::isAtEnd() -> bool {
    return peek().eTokenType == ETokenType::END_OF_FILE;
};

auto Parser::advance() -> Token {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
};

auto Parser::check(ETokenType type) -> bool {
    if (isAtEnd()) {
        return false;
    }
    return peek().eTokenType == type;
};

auto Parser::match(std::initializer_list<ETokenType> types) -> bool {
    for (const ETokenType& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
};

auto Parser::synchronize() -> void {
    advance();

    while (!isAtEnd()) {
        if (previous().eTokenType == ETokenType::SEMICOLON) {
            return;
        }
        switch (peek().eTokenType) {
        case ETokenType::CLASS:
        case ETokenType::FUN:
        case ETokenType::VAR:
        case ETokenType::FOR:
        case ETokenType::IF:
        case ETokenType::WHILE:
        case ETokenType::PRINT:
        case ETokenType::RETURN: return;
        }

        advance();
    }
};

auto Parser::consume(ETokenType type, const std::string& message) -> Token {
    if (check(type)) {
        return advance();
    }
    Error::error(peek(), message);
    throw std::runtime_error{"exception thrown"};
};

auto Parser::primary() -> Expr {
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
        consume(ETokenType::RIGHT_PARENTHESIS, "Expect ')' after expression.");
        return Grouping(expr);
    }
    Error::error(peek(), "Expect expression.");
    throw std::runtime_error("Expect expression.");
};

auto Parser::unary() -> Expr {
    if (match({ETokenType::BANG, ETokenType::MINUS})) {
        Token _operator = previous();
        Expr right = unary();
        return Unary(_operator, right);
    }

    return primary();
};

auto Parser::multiplication() -> Expr {
    Expr expr = unary();

    while (match({ETokenType::SLASH, ETokenType::STAR})) {
        Token _operator = previous();
        Expr right = unary();
        return Binary(expr, _operator, right);
    }

    return expr;
};

auto Parser::addition() -> Expr {
    Expr expr = multiplication();

    while (match({ETokenType::MINUS, ETokenType::PLUS})) {
        Token _operator = previous();
        Expr right = multiplication();
        expr = Binary(expr, _operator, right);
    }

    return expr;
};

auto Parser::comparison() -> Expr {
    Expr expr = addition();

    while (match({ETokenType::GREATER, ETokenType::GREATER_EQUAL,
                  ETokenType::LESS, ETokenType::LESS_EQUAL})) {
        Token _operator = previous();
        Expr right = addition();
        expr = Binary(expr, _operator, right);
    }

    return expr;
};

auto Parser::equality() -> Expr {
    Expr expr = comparison();

    while (match({ETokenType::BANG_EQUAL, ETokenType::EQUAL_EQUAL})) {
        Token _operator = previous();
        Expr right = comparison();
        expr = Binary(expr, _operator, right);
    }

    return expr;
};

auto Parser::expression() -> Expr {
    return equality();
};

} // namespace cpplox
