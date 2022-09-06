#include "ExpressionParser.h"
#include "Error.h"
#include "ExpressionPrinterVisitor.h"

#include <array>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
//#include <variant>

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
    if (match({ETokenType::FOR})) {
        return forStatement();
    }
    if (match({ETokenType::IF})) {
        return ifStatement();
    }

    if (match({ETokenType::PRINT})) {
        return printStatement();
    }
    if (match({ETokenType::RETURN})) {
        return returnStatement();
    }

    if (match({ETokenType::WHILE})) {
        return whileStatement();
    }
    if (match({ETokenType::LEFT_BRACE})) {
        return BlockStatement(block());
    }
    return expressionStatement();
}

auto Parser::forStatement() -> Statement {
    consume(ETokenType::LEFT_PARENTHESIS, "Expect '(' after 'for'.");
    Statement initializer = nullptr;
    if (match({ETokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({ETokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    Expr condition = nullptr;
    if (!check(ETokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(ETokenType::SEMICOLON, "Expect ';' after loop condition.");

    Expr increment = nullptr;
    if (!check(ETokenType::RIGHT_PARENTHESIS)) {
        increment = expression();
    }
    consume(ETokenType::RIGHT_PARENTHESIS, "Expect ')' after for clauses.");

    Statement body = statement();

    // insert the increment into a new block statment
    if (increment != nullptr) {
        // small optimization to modify existing block if one already exists to
        // prevent env pushing.
        if (is<BlockStatement>(body)) {
            getAs<BlockStatement>(body).statements.emplace_back(
                ExpressionStatement{increment});
        } else {
            // if it was just a single statement (no { } ) then make a block
            // statment. there will still be no env push due to while statements
            // knowing not to do that.
            body = BlockStatement({body, ExpressionStatement{increment}});
        }
    }

    // if there is no condition specified then we want to make the while loop
    // continue indefinetly
    if (condition == nullptr) {
        condition = Literal(true);
    }

    // replace body with the while statement
    body = WhileStatement(condition, body);

    // if there is an init statement, then we have to prepend the while with it.
    if (initializer != nullptr) {
        body = BlockStatement({initializer, body});
    }

    return body;
}

auto Parser::whileStatement() -> Statement {
    consume(ETokenType::LEFT_PARENTHESIS, "Expect '(' after 'while'");
    Expr condition = expression();
    consume(ETokenType::RIGHT_PARENTHESIS, "Expect ')' after 'while'");
    Statement body = statement();

    return WhileStatement(condition, body);
}

auto Parser::function(std::string kind) -> Statement {
    Token name = consume(ETokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(ETokenType::LEFT_PARENTHESIS,
            "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(ETokenType::RIGHT_PARENTHESIS)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Cannot have more than 255 parameters.");
            }

            parameters.push_back(
                consume(ETokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({ETokenType::COMMA}));
    }
    consume(ETokenType::RIGHT_PARENTHESIS, "Expect ')' after parameters.");

    consume(ETokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<Statement> body = block();
    return FunctionStatement(name, parameters, body);
}

auto Parser::declaration() -> Statement {
    try {
        if (match({ETokenType::FUN})) {
            return function("function");
        }
        if (match({ETokenType::VAR})) {
            return varDeclaration();
        }
        return statement();
    } catch (const ParseError&) {
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

auto Parser::returnStatement() -> Statement {
    Token keyword = previous();
    Expr value = nullptr;
    if (!check(ETokenType::SEMICOLON)) {
        value = expression();
    }

    consume(ETokenType::SEMICOLON, "Expect ';' after return value.");
    return ReturnStatement(keyword, value);
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
        auto lit = Literal(previous().literal);
        return lit;
    }

    if (match({ETokenType::IDENTIFIER})) {
        return Variable(previous());
    }

    if (match({ETokenType::LEFT_PARENTHESIS})) {
        Expr expr = expression();
        consume(ETokenType::RIGHT_PARENTHESIS, "Expect ')' after expression.");
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

    return prefix();
    //return primary();
};

auto Parser::call() -> Expr {
    Expr expr = primary();

    while (true) {
        if (match({ETokenType::LEFT_PARENTHESIS})) {
            expr = finishCall(expr);
        } else {
            break;
        }
    }

    return expr;
}

auto Parser::finishCall(Expr callee) -> Expr {
    std::vector<Expr> arguments;
    if (!check(ETokenType::RIGHT_PARENTHESIS)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Cannot have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({ETokenType::COMMA}));
    }

    Token paren = consume(ETokenType::RIGHT_PARENTHESIS,
                          "Expect right parens after arguments.");

    return Call(callee, paren, arguments);
}

auto Parser::multiplication() -> Expr {
    Expr expr = unary();

    while (match({ETokenType::SLASH, ETokenType::STAR, ETokenType::MOD})) {
        Token _operator = previous();
        Expr right = unary();
        expr = Binary(expr, _operator, right);
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

        if (expr.is<Variable>()) {
            const Variable& variable = expr.get<Variable>();
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

auto Parser::prefix() -> Expr {


    if (match({ETokenType::PLUS_PLUS, ETokenType::MINUS_MINUS})) {
        Token op = previous();
        // call to the next as prefix to set the right precedence. we'll also
        // check that we dont chain these
        Expr right = prefix();
        if (right.is<Increment>() || right.is<Decrement>()) {
            throw Parser::error(peek(), "Cannot concatenate inc/dec operator.");
        }
        if (right.is<Variable>()) {
            if (op.eTokenType == ETokenType::PLUS_PLUS) {
                return Increment{op, right, Increment::Type::PREFIX};
            } else {
                return Decrement{op, right, Increment::Type::PREFIX};
            }
        } else {
            throw Parser::error(peek(),
                                "Operators '++' and '--' must be applied to an "
                                "lvalue operand (a variable)");
        }
    }

    return postfix();
}

auto Parser::postfix() -> Expr {

    Expr expr = call();
    if (match({ETokenType::PLUS_PLUS, ETokenType::MINUS_MINUS})) {
        Token op = previous();
        if (expr.is<Variable>()) {
            if (op.eTokenType == ETokenType::PLUS_PLUS) {
                expr = Increment{op, expr, Increment::Type::POSTFIX};
            } else {
                expr = Decrement{op, expr, Increment::Type::POSTFIX};
            }
        } else {
            throw Parser::error(peek(),
                  "Operators '++' and '--' must be applied to an lvalue "
                  "operand (a variable)");
        }
    }

    if (match({ETokenType::PLUS_PLUS, ETokenType::MINUS_MINUS})) {
        error(peek(), "Operators '++' and '--' cannot be concatenated");
    }

    return expr;
}

} // namespace cpplox
