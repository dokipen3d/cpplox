#pragma once
#include "Expr.hpp"
#include "TokenTypes.h"
#include "Utilities.hpp"

namespace cpplox {

struct VoidType {};
// recursive block
struct BlockStatement;
struct IfStatement;

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
    VariableStatement(Token name, Expr initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {
    }
    Token name;
    Expr initializer;
};

using Statement =
    std::variant<ExpressionStatement, PrintStatement, VariableStatement,
                 recursive_wrapper<BlockStatement>,
                 recursive_wrapper<IfStatement>, VoidType*>;

// helper functions to make variant comparable to nullptr
//////////////////////////////////////////////////////////////////////////
inline bool operator==(
    const Statement& other,
    std::nullptr_t) { // needs to be inline because its a free function that
                      // it included in multiple translation units. needs to
                      // be marked inline so linker knows its the same one
    return std::holds_alternative<VoidType*>(other);
}

inline bool operator!=(const Statement& other, std::nullptr_t ptr) {
    return !(other == ptr);
}

struct BlockStatement {
    BlockStatement(std::vector<Statement> statements)
        : statements(std::move(statements)) {
    }
    std::vector<Statement> statements;
};

struct IfStatement {
    IfStatement(Expr condition, Statement thenBranch, Statement elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
    }
    Expr condition;
    Statement thenBranch;
    Statement elseBranch;
};

} // namespace cpplox