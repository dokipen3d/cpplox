#pragma once
#include "TokenTypes.h"
#include "Expr.hpp"


namespace cpplox {

struct VoidStatement{};

struct ExpressionStatement {
    explicit ExpressionStatement(Expr expression)
        : expression(std::move(expression)) {
    }
    Expr expression;
};

struct PrintStatement {
    explicit PrintStatement(Expr expression)
        : expression(std::move(expression)) {
    }
    Expr expression;
};

struct VariableStatement {
    VariableStatement(Token nameIn, Expr initializer)
        : nameOfVariable(std::move(nameIn)), initializer(std::move(initializer)) {
        }
    Token nameOfVariable;
    Expr initializer;
};

using Statement = std::variant<ExpressionStatement, PrintStatement,
                               VariableStatement, VoidStatement>;

} // namespace cpplox