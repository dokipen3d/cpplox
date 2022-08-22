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
    explicit ExpressionStatement(const Expr& expression)
        : expression(expression) {
    }
    Expr expression;
};

struct PrintStatement {
    explicit PrintStatement(const Expr& expression)
        : expression(expression) {
    }
    Expr expression;
};

struct VariableStatement {
    VariableStatement(const Token& name, const Expr& initializer)
        : name(name), initializer(initializer) {
    }
    Token name;
    Expr initializer;
};

struct ReturnStatement {
    ReturnStatement(const Token& name, const Expr& value)
        : name(name), value(value) {
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
    explicit BlockStatement(const std::vector<Statement>& statements, const ExpressionStatement& increment)
        : statements(statements), increment(increment) {
    }
    std::vector<Statement> statements;
    ExpressionStatement increment;
    bool containsIncrement = false;

};

struct FunctionStatement {
    FunctionStatement(const Token& p_name, const std::vector<Token>& params,
                      const std::vector<Statement>& body)
        : name(p_name), params(params),
          body(body) {
    }

    Token name;
    std::vector<Token> params;
    std::vector<Statement> body;
};

struct IfStatement {
    IfStatement(const Expr& condition, const Statement& thenBranch, const Statement& elseBranch)
        : condition(condition), thenBranch(thenBranch),
          elseBranch(elseBranch) {
    }
    Expr condition;
    Statement thenBranch;
    Statement elseBranch;
};

struct WhileStatement {
    WhileStatement(const Expr& condition, const Statement& body)
        : condition(condition), body(body) {
    }
    Expr condition;
    Statement body;
};

} // namespace cpplox