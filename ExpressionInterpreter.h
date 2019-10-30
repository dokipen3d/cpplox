#pragma once
#include "Environment.h"
#include "Expr.hpp"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include <memory>
#include <vector>

namespace cpplox {
struct Interpreter {
    Interpreter::Interpreter()  {
        environmentStack.emplace_back(environmentStack, -1);
    }
    void interpret(const std::vector<Statement>& statements);
    void execute(const Statement& statementToExecute);
    Object evaluate(const Expr& expression);
    void operator()(const ExpressionStatement& expressionStatement);
    void operator()(const PrintStatement& printStatement);
    void operator()(const VariableStatement& variableStatement);
    void operator()(const VoidStatement neverCalled) {
    }
    void operator()(const BlockStatement& blockStatement);
    void executeBlock(const std::vector<Statement>& statements);

    Object operator()(const Assign& assign);
    Object operator()(const Binary& binary);
    Object operator()(const Literal& literal);
    Object operator()(const Grouping& grouping);
    Object operator()(const Unary& unary);
    Object operator()(const Variable& variable);

    Object operator()(const void* neverCalled) {
        return nullptr;
    }

    bool isTruthy(const Object& object);
    bool isEqual(const Object& a, const Object& b);
    // we could rely on the bad_variant_access but this way we throw based on
    // type. might be slower. worth investigating in future.
    void checkNumberOperand(const Token& token, const Object& operand);
    // version of the fuction for binary operators
    void checkNumberOperands(const Token& token, const Object& left,
                             const Object& right);
    std::string stringify(const Object& object);
    std::shared_ptr<Environment>
        environment; // this maybe overriden temporarily by blocks and then set
                     // back
    std::vector<Environment> environmentStack;
    int currentEnvironmentIndex = 0;
    const TimeIt timeIt;
};
} // namespace cpplox