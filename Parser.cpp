#include "Expression.h"

#include "Parser.h"
#include <string>

namespace cpplox {

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

        //throw error(peek(), message);
    };

    auto primary = [&]() -> Expr {
        if (match({ETokenType::FALSE}))
            return Literal(false);
        if (match({ETokenType::TRUE}))
            return Literal(true);
        if (match({ETokenType::NIL}))
            return Literal(nullptr);

        if (match({ETokenType::NUMBER, ETokenType::STRING})) {
            return Literal(previous().literal);
        }

        if (match({ETokenType::LEFT_PAREN})) {
            Expr expr = expression();
            consume(ETokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return Expr.Grouping(expr);
        }
    };

    auto unary = [&]() -> Expr {
        if (match({ETokenType::BANG, ETokenType::MINUS})) {
            Token _operator = previous();
            Expr right = unary();
            return Expr.Unary(_operator, right);
        }

        return primary();
    };

    auto multiplication = [&]() -> Expr {
        Expr expr = unary();

        while (match({ETokenType::SLASH, ETokenType::SLASHSTAR})) {
            Token _operator = previous();
            Expr right = unary();
            expr = Expr.Binary(expr, _operator, right);
        }

        return expr;
    };

    auto addition = [&]() -> Expr {
        Expr expr = multiplication();

        while (match({ETokenType::MINUS, ETokenType:: : PLUS})) {
            Token _operator = previous();
            Expr right = multiplication();
            expr = Expr.Binary(expr, _operator, right);
        }

        return expr;
    };

    auto comparison = [&]() -> Expr {
        Expr expr = addition();

        while (match({ETokenType::GREATER, ETokenType::GREATER_EQUAL,
                      ETokenType::LESS, ETokenType::LESS_EQUAL})) {
            Token _operator = previous();
            Expr right = addition();
            expr = Expr.Binary(expr, _operator, right);
        }

        return expr;
    };

    auto equality = [&]() -> Expr {
        Expr expr = comparison();

        while (match({ETokenType::BANG_EQUAL, ETokenType::EQUAL_EQUAL})) {
            Token _operator = previous();
            Expr right = comparison();
            expr = Expr.Binary(expr, _operator, right);
        }

        return expr;
    };

    auto expression = [&]() -> Expr { return equality(); };
}

} // namespace cpplox