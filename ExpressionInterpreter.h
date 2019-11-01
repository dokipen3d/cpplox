#pragma once
#include "Environment.h"
#include "Expr.hpp"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include <memory>
#include <vector>

namespace cpplox {
struct Interpreter {
    Interpreter() : environment(std::make_unique<Environment>()) {
    }
    void interpret(const std::vector<Statement>& statements);
    void execute(const Statement& statementToExecute);
    Object evaluate(const Expr& expression);
    void operator()(const ExpressionStatement& expressionStatement);
    void operator()(const IfStatement& ifStatement);
    void operator()(const WhileStatement& whileStatement);


    void operator()(const PrintStatement& printStatement);
    void operator()(const VariableStatement& variableStatement);
    void operator()(const VoidType*) {}


    void operator()(const BlockStatement& blockStatement);
    void executeBlock(const std::vector<Statement>& statements);

    Object operator()(const Assign& assign);
    Object operator()(const Binary& binary);
    Object operator()(const Literal& literal);
    Object operator()(const Grouping& grouping);
    Object operator()(const Unary& unary);
    Object operator()(const Variable& variable);
    Object operator()(const Logical& variable);


    Object operator()(const void*) {
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
    std::unique_ptr<Environment>
        environment; // this maybe overriden temporarily by blocks and then set
                     // back
    const TimeIt timeIt;
};
} // namespace cpplox