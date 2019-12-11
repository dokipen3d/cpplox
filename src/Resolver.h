#pragma once
#include "Expr.hpp"
#include "Statement.hpp"
#include "TokenTypes.h"
#include "Utilities.hpp"
#include <map>
#include <string>
#include <vector>

namespace cpplox {

struct Interpreter;

struct Resolver {
    Resolver(const Interpreter& interpreter);
    void resolve(const Statement& statement);
    void resolve(const std::vector<Statement>& statements);
    void resolve(const Expr& expr);
    void resolveLocal(const Expr& expr, const Token& name);  
    void resolveFunction(const FunctionStatement& FunctionStatement); 

    void declare(const Token& name);
    void define(const Token& name);

    void beginScope();
    void endScope();


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
    void operator()(const void*) {
    }
    const Interpreter& m_interpreter;
    std::vector<std::map<std::string, bool>> scopes;
};

} // namespace cpplox