#pragma once
#include "Expr.hpp"
#include "TokenTypes.h"
#include "Utilities.hpp"

namespace cpplox {

struct VoidStatement {};
//recursive block
struct BlockStatement;

struct ExpressionStatement {
    explicit ExpressionStatement(Expr expression) : expression(std::move(expression)) {
    }
    Expr expression;
};

struct PrintStatement {
    explicit PrintStatement(Expr expression) : expression(std::move(expression)) {
    }
    Expr expression;
};

struct VariableStatement {
    VariableStatement(Token name, Expr initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {
    }
    Token name;
    Expr initializer;
};



using Statement =
    std::variant<ExpressionStatement, PrintStatement, VariableStatement, VoidStatement, recursive_wrapper<BlockStatement>>;

struct BlockStatement {
    BlockStatement(std::vector<Statement> statements) : statements(std::move(statements)) {
    }
    std::vector<Statement> statements;
};

} // namespace cpplox