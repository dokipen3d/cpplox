#pragma once
#include "Expr.hpp"
#include "TokenTypes.h"

#include "Utilities.hpp"

namespace cpplox {

struct VoidType {};
// recursive block
struct BlockStatement;
struct IfStatement;
struct WhileStatement;
struct FunctionStatement;

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

struct ReturnStatement {
    ReturnStatement(Token name, Expr value)
        : name(std::move(name)), value(std::move(value)) {
    }
    Token name;
    // an expression that resolves to the value that we want to return
    Expr value;
};

using Statement =
    std::variant<ExpressionStatement, PrintStatement, VariableStatement,
                 ReturnStatement, recursive_wrapper<BlockStatement>,
                 recursive_wrapper<IfStatement>,
                 recursive_wrapper<WhileStatement>,
                 recursive_wrapper<FunctionStatement>, VoidType*>;

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

// helper for hold_alternative to unwrap the wrapper
template <typename T>
bool is(const Statement& statement) { // function needs to be const  to make it
                                      // callable from a const ref
    return std::holds_alternative<recursive_wrapper<T>>(statement);
}

template <typename T>
T& getAs(Statement& statement) { // function needs to be const  to make it
                                 // callable from a const ref
    return std::get<recursive_wrapper<T>>(statement);
}

template <typename T> T& get(Statement& statement) {
    // constexpr bool isRecursive = ;
    using actualTypeToGet =
        std::conditional_t<has_type_v<recursive_wrapper<T>, Statement>,
                           recursive_wrapper<T>, T>;
    return std::get<actualTypeToGet>(statement);
}

//////////////////////////////////////////////////////////////////////////

struct BlockStatement {
    explicit BlockStatement(std::vector<Statement> statements)
        : statements(std::move(statements)) {
    }
    std::vector<Statement> statements;
};

struct FunctionStatement {
    FunctionStatement(Token p_name, std::vector<Token> params,
                      std::vector<Statement> body)
        : name(std::move(p_name)), params(std::move(params)),
          body(std::move(body)) {
    }

    Token name;
    std::vector<Token> params;
    std::vector<Statement> body;
};

struct IfStatement {
    IfStatement(Expr condition, Statement thenBranch, Statement elseBranch)
        : condition(condition), thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {
    }
    Expr condition;
    Statement thenBranch;
    Statement elseBranch;
};

struct WhileStatement {
    WhileStatement(Expr condition, Statement body)
        : condition(condition), body(std::move(body)) {
    }
    Expr condition;
    Statement body;
};

} // namespace cpplox