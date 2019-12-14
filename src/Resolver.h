#pragma once
#include "Expr.hpp"
#include "Statement.hpp"
#include "TokenTypes.h"
#include "Utilities.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace cpplox {
    
struct Resolver {
    Resolver(){}
    void resolve(const Statement& statement);
    void resolve(const std::vector<Statement>& statements);
    void resolve(const Expr& expr);
    void resolveFunction(const FunctionStatement& FunctionStatement);

    void declare(const Token& name);
    void define(const Token& name);

    void beginScope();
    void endScope();
    void resolveLocal(const Variable& expr, const Token& name);
    void resolveLocal(const Assign& expr, const Token& name);


    void operator()(const ExpressionStatement& expressionStatement);
    void operator()(const IfStatement& ifStatement);
    void operator()(const WhileStatement& whileStatement);
    void operator()(const FunctionStatement& functionStatement);
    void operator()(const PrintStatement& printStatement);
    void operator()(const VariableStatement& variableStatement);
    void operator()(const ReturnStatement& returnStatement);
    void operator()(const VoidType*) {
    }
    void operator()(const BlockStatement& blockStatement);

    void operator()(const Assign& assign);
    void operator()(const Binary& binary);
    void operator()(const Literal& literal);
    void operator()(const Grouping& grouping);
    void operator()(const Unary& unary);
    void operator()(const Variable& variable);
    void operator()(const Logical& logical);
    void operator()(const Call& call);
    void operator()(const ExprVoidType*) {
    }
    bool enableEnvironmentSwitching = true;
    std::vector<std::unordered_map<std::string, bool>> scopes;
};

} // namespace cpplox