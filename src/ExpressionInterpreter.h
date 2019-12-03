#pragma once
#include "Environment.h"
#include "Expr.hpp"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include <chrono>
#include <exception>
#include <memory>
#include <vector>

struct Object;

namespace cpplox {

// we hijack c++ exceptions to be able to unwind a stack and jump back to a
// particular point in code and embed a value
struct Return : std::exception {

    Return(Object value) : value(std::move(value)) {
    }
    Object value;
};

struct Interpreter {
    Interpreter()
        : environment(std::make_unique<Environment>()),
          globals(environment.get()) {

        // should move this to cpp file....

        globals->define(
            "clock",
            NativeFunction{
                /*call*/ [](const Interpreter& interpreter,
                            const std::vector<Object> arguments) -> Object {
                    using namespace std::chrono;
                    double s =
                        duration<double>(system_clock::now().time_since_epoch())
                            .count();
                    return s;
                },
                /*arity*/ []() { return 0; }});
    }

    void interpret(const std::vector<Statement>& statements);
    void execute(const Statement& statementToExecute);
    Object evaluate(const Expr& expression);
    void operator()(const ExpressionStatement& expressionStatement);
    void operator()(const IfStatement& ifStatement);
    void operator()(const WhileStatement& whileStatement);
    void operator()(const FunctionStatement& functionStatement);

    void operator()(const PrintStatement& printStatement);
    void operator()(const VariableStatement& variableStatement);
    void operator()(const ReturnStatement& variableStatement);

    void operator()(const VoidType*) {
    }

    void operator()(const BlockStatement& blockStatement);
    void executeBlock(const std::vector<Statement>& statements,
                      std::unique_ptr<Environment> newEnvironement);

    Object operator()(const Assign& assign);
    Object operator()(const Binary& binary);
    Object operator()(const Literal& literal);
    Object operator()(const Grouping& grouping);
    Object operator()(const Unary& unary);
    Object operator()(const Variable& variable);
    Object operator()(const Logical& logical);
    Object operator()(const Call& call);

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
    Environment* globals; // place to store global native functions etc
    const TimeIt timeIt;
    bool enableEnvironmentSwitching =
        true; // when looping, we dont need to push and pop environments so we
              // disable
};
} // namespace cpplox