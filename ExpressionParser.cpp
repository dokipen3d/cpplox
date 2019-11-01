#include "ExpressionParser.h"
#include "Error.h"
#include "ExpressionPrinterVisitor.h"

#include <array>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

namespace cpplox {

Parser::Parser(std::vector<Token>& tokens) : tokens(tokens) {
}

void Parser::print(const Expr& expr) {
    ExpressionPrinterVisitor v;
    v.print(expr);
}

// to be able to still evaluate expressions, we can count the number of ';'s
// in our tokens and if there are none, fall back to just straight eval
auto Parser::parseExpression() -> Expr {
    try {
        Expr expr = expression();
        // extra check to find if there is a trailing token that didnt get
        // parsed
        if (tokens[current].eTokenType != ETokenType::END_OF_FILE) {
            Error::error(peek(), "Expect expression. got " +
                                     tokens[current].toString());
            throw cpplox::ParseError("Expect expression.");
        }
        return expr;
    } catch (const cpplox::ParseError& error) {
        std::cout << "caught! " << error.what() << "\n";
        return nullptr;
    }
}

auto Parser::parse() -> std::vector<Statement> {
    std::vector<Statement> statements;

    // maybe we can prescan the tokens and reserve the statements. maybe even
    // use unique_ptr<Statement[]> as per herb sutter's advice for dynamics but
    // fixed size storage
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }

    return statements;
}

auto Parser::error(Token token, std::string message) -> ParseError {
    Error::error(token, message);
    return ParseError("parser error");
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
    throw Parser::error(peek(), message);
};

auto Parser::statement() -> Statement {
    if (match({ETokenType::IF})) {
        return ifStatement();
    }

    if (match({ETokenType::PRINT})) {
        return printStatement();
    }
    if (match({ETokenType::WHILE})) {
        return whileStatement();
    }
    if (match({ETokenType::LEFT_BRACE})) {
        return BlockStatement(block());
    }
    return expressionStatement();
}

auto Parser::whileStatement() -> Statement {
    consume(ETokenType::LEFT_PARENTHESIS, "Expect '(' after 'while'");
    Expr condition = expression();
    consume(ETokenType::RIGHT_PARENTHESIS, "Expect ')' after 'while'");
    Statement body = statement();

    return WhileStatement(condition, body);
}

auto Parser::declaration() -> Statement {
    try {
        if (match({ETokenType::VAR})) {
            return varDeclaration();
        }
        return statement();
    } catch (const ParseError& error) {
        synchronize();
        return nullptr;
    }
}

auto Parser::varDeclaration() -> Statement {
    Token name = consume(ETokenType::IDENTIFIER, "Expect variable name");

    Expr initializer = nullptr;
    if (match({ETokenType ::EQUAL})) {
        initializer = expression();
    }

    consume(ETokenType::SEMICOLON, "Expect ';' after variable declaration");
    return VariableStatement(name, initializer);
}

auto Parser::ifStatement() -> Statement {
    consume(ETokenType::LEFT_PARENTHESIS, "Expect '(' after 'if'.");
    Expr condition = expression();
    consume(ETokenType::RIGHT_PARENTHESIS, "Expect ')' after if condition.");

    Statement thenBranch = statement();
    Statement elseBranch = nullptr;
    if (match({ETokenType::ELSE})) {
        elseBranch = statement();
    }

    return IfStatement(condition, thenBranch, elseBranch);
}
auto Parser::block() -> std::vector<Statement> {
    std::vector<Statement> statements;
    while (!check(ETokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(ETokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

auto Parser::printStatement() -> Statement {
    Expr value = expression();
    consume(ETokenType::SEMICOLON, "Expect ';' after value.");
    return PrintStatement(value);
}

auto Parser::expressionStatement() -> Statement {
    Expr expr = expression();
    consume(ETokenType::SEMICOLON, "Expect ';' after expression.");
    return ExpressionStatement(expr);
}

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

    if (match({ETokenType::IDENTIFIER})) {
        return Variable(previous());
    }

    if (match({ETokenType::LEFT_PARENTHESIS})) {
        Expr expr = expression();
        (ETokenType::RIGHT_PARENTHESIS, "Expect ')' after expression.");
        return Grouping(expr);
    }
    throw Parser::error(peek(), "Expect expression.");
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
    return assignment();
};

auto Parser::logicalAnd() -> Expr {
    Expr expr = equality();

    while (match({ETokenType::AND})) {
        Token oPerator = previous();
        Expr right = equality();
        expr = Logical(expr, oPerator, right);
    }

    return expr;
}

auto Parser::logicalOr() -> Expr {
    Expr expr = logicalAnd();

    while (match({ETokenType::OR})) {
        Token oPerator = previous();
        Expr right = logicalAnd();
        expr = Logical(expr, oPerator, right);
    }

    return expr;
}

auto Parser::assignment() -> Expr {
    Expr expr = logicalOr();

    if (match({ETokenType::EQUAL})) {
        Token equals = previous();
        Expr value = assignment();

        if (is<Variable>(expr)) {
            const Variable& variable = expr_get<Variable>(expr);
            Token name = variable.name;
            return Assign(name, value);
        }
        // We report an error if the left-hand side isn’t a valid assignment
        // target, but we don’t throw it because the parser isn’t in a confused
        // state where we need to go into panic mode and synchronize.
        error(equals, "Invalid assignment target.");
    }
    return expr;
};

} // namespace cpplox
