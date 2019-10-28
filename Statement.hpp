#pragma once
#include "Expr.hpp"

namespace cpplox {

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

using Statement =
    std::variant<ExpressionStatement, PrintStatement>;

} // namespace cpplox