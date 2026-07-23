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
struct ClassStatement;

struct ExpressionStatement {
    explicit ExpressionStatement(const Expr& expression)
        : expression(expression) {
    }
    Expr expression;
};

struct PrintStatement {
    explicit PrintStatement(const Expr& expression) : expression(expression) {
    }
    Expr expression;
};

struct VariableStatement {
    VariableStatement(const Token& name, const Expr& initializer, const Token& typeName)
        : name(name), initializer(initializer), typeName(typeName) {
    }
    Token name;
    Expr initializer;
    Token typeName;

};

struct ReturnStatement {
    ReturnStatement(const Token& name, const Expr& value)
        : name(name), value(value) {
    }
    Token name;

    // an expression that resolves to the value that we want to return
    Expr value;
};

using StatementVariant = std::variant<
    ExpressionStatement, PrintStatement, VariableStatement, ReturnStatement,
    recursive_wrapper<BlockStatement>, recursive_wrapper<IfStatement>,
    recursive_wrapper<WhileStatement>, recursive_wrapper<FunctionStatement>,
    recursive_wrapper<ClassStatement>, VoidType*>;

struct Statement final : StatementVariant {

    using StatementVariant::StatementVariant;
    using StatementVariant::operator=;

    // Statement(Statement const&) = default;
    // Statement(Statement&&) = default;

    // Statement& operator=(const Statement& other) = default;
    // Statement& operator=(Statement&& other) = default;

};

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
struct ClassStatement {
    ClassStatement( const Token& name,
                    const std::vector<Statement>& functionStatements,
                    const std::vector<Statement>& functionProperties)
        : name(name), functionStatements(functionStatements), functionProperties(functionProperties){
    }
    Token name;
    std::vector<Statement> functionStatements;
    std::vector<Statement> functionProperties;
};

struct BlockStatement {
    explicit BlockStatement(const std::vector<Statement>& statements)
        : statements(std::move(statements)) {
    }
    std::vector<Statement> statements;
};

struct FunctionStatement {
    FunctionStatement(const Token& p_name, const std::vector<Token>& params,
                      const std::vector<Statement>& body, const Token& returnType)
        : name(p_name), params(params), body(body), returnType(returnType) {
    }

    Token name;
    Token returnType;
    std::vector<Token> params;
    std::vector<Statement> body;
};

struct IfStatement {
    IfStatement(const Expr& condition, const Statement& thenBranch,
                const Statement& elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
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



// template<>
// inline std::vector<std::vector<cpplox::BlockStatement>> cpplox::recursive_wrapper<cpplox::BlockStatement>::t{};
// template<>
// inline std::vector<std::vector<cpplox::IfStatement>> cpplox::recursive_wrapper<cpplox::IfStatement>::t{};
// template<>
// inline std::vector<std::vector<cpplox::WhileStatement>> cpplox::recursive_wrapper<cpplox::WhileStatement>::t{};

static inline int createScriptStatement(){


    cpplox::recursive_wrapper<cpplox::BlockStatement>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::IfStatement>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::WhileStatement>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::FunctionStatement>::t.push_back({});
    cpplox::recursive_wrapper<cpplox::ClassStatement>::t.push_back({});
    // cpplox::recursive_wrapper<cpplox::ExpressionStatement>::t.push_back({});
    // cpplox::recursive_wrapper<cpplox::PrintStatement>::t.push_back({});
    // cpplox::recursive_wrapper<cpplox::VariableStatement>::t.push_back({});
    // cpplox::recursive_wrapper<cpplox::ReturnStatement>::t.push_back({});



    return cpplox::recursive_wrapper<cpplox::WhileStatement>::t.size();
}

static inline void clearStorageStatement(){
   
    cpplox::recursive_wrapper<cpplox::BlockStatement>::t.clear();
    cpplox::recursive_wrapper<cpplox::IfStatement>::t.clear();
    cpplox::recursive_wrapper<cpplox::WhileStatement>::t.clear();
}

} // namespace cpplox

template <>
struct std::variant_size<cpplox::Statement>
    : std::variant_size<cpplox::StatementVariant> {};

template <std::size_t I>
struct std::variant_alternative<I, cpplox::Statement>
    : std::variant_alternative<I, cpplox::StatementVariant> {};

