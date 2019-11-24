#pragma once
#include "Environment.h"
#include "Expr.hpp"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include <chrono>
#include <memory>
#include <vector>

namespace cpplox {
struct Interpreter {
    Interpreter()
        : globals(std::make_unique<Environment>()),
          environment(std::make_unique<Environment>(globals.get())) {
        // should move this to cpp file....
        NativeFunction f = NativeFunction{
                /*call*/ [](const Interpreter& interpreter,
                            const std::vector<Object> arguments) -> Object {
                    std::cout << "inside func \n";

                    using namespace std::chrono;
                    double s = duration<double>(
                        system_clock::now().time_since_epoch()).count();
                    std::cout << s << "\n";

                    return 1.2;
                },
                /*arity*/ []() { return 0; }};;

        f.m_func(*this,{});

        globals->define(
            "clock",
            f);
        Object c = globals->get(Token(ETokenType::FUN, "clock", nullptr, 0));
        Object o = c.get<NativeFunction>().m_func(*this, {});
        std::cout << stringify(o);
		

    }

    void interpret(const std::vector<Statement>& statements);
    void execute(const Statement& statementToExecute);
    Object evaluate(const Expr& expression);
    void operator()(const ExpressionStatement& expressionStatement);
    void operator()(const IfStatement& ifStatement);
    void operator()(const WhileStatement& whileStatement);

    void operator()(const PrintStatement& printStatement);
    void operator()(const VariableStatement& variableStatement);
    void operator()(const VoidType*) {
    }

    void operator()(const BlockStatement& blockStatement);
    void executeBlock(const std::vector<Statement>& statements);

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
        globals; // place to store global native functions etc
    std::unique_ptr<Environment>
        environment; // this maybe overriden temporarily by blocks and then set
                     // back
    const TimeIt timeIt;
    bool enableEnvironmentSwitching =
        true; // when looping, we dont need to push and pop environments so we
              // disable
};
} // namespace cpplox