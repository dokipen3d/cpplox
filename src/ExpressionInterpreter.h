#pragma once
#include "Environment.h"
#include "Expr.hpp"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include "thirdparty/sparestack.hpp"
// #include "thirdparty/shared_ptr.hpp"
#include "boost/smart_ptr/local_shared_ptr.hpp"
#include <chrono>
#include <exception>
#include <map>
#include <memory>
#include <unordered_map>
#include <variant>
// #include "thirdparty/mvariant.hpp"

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

struct ObjectAdder {

    Object operator()(const double a, const double b) const;
    //Object operator()(const wrapper<std::string>& a, const wrapper<std::string>& b) const;

    // Handle other combinations, possibly with errors if they're invalid
    template<typename T, typename U>
    Object operator()(const T&, const U&) const {
        throw std::runtime_error("Type mismatch error");
    }
};

struct ObjectSubber {
    ObjectSubber() = default;
    Object operator()(const double& a, const double& b);
    Object operator()(const double& a, const double& b) const;


    // Handle other combinations, possibly with errors if they're invalid
    template<typename T, typename U>
    Object operator()(const T&, const U&) const {
        throw std::runtime_error("Type mismatch error");
    }

     template<typename T, typename U>
    Object operator()(const T&, const U&) {
        throw std::runtime_error("Type mismatch error");
    }

};

struct Interpreter {
    Interpreter() {
        globals = retrieveEnvironment();
        environment = globals;
        // should move this to cpp file....
        globals->define(
            FNVHash("clock"), // as long as this hash matches the one made in
                              // the token constructor we should be okay, we
                              // might need to add rehashing to token
                              // constructor to prevent collisions
                              // we can also create an overloaded define for env
                              // tht takes the string and does the hash for us
            NativeFunction{
                /*call*/ [](const Interpreter& interpreter,
                            const std::vector<Object>& arguments) -> Object {
                    using namespace std::chrono;
                    double s =
                        duration<double>(system_clock::now().time_since_epoch())
                            .count();
                    return s;
                },
                /*arity*/ []() { return 0; }});

        globals->define(
            FNVHash("str"),
            NativeFunction{
                /*call*/ [](const Interpreter& interpreter,
                            const std::vector<Object>& arguments) -> Object {
                    std::string s = std::to_string(arguments[0].get<double>());
                    stripZerosFromString(s);
                    return s;
                },
                /*arity*/ []() { return 1; }});
    }

    void interpret(const std::vector<Statement>& statements);
    void execute(const Statement& statementToExecute);
    Object evaluate(const Expr& expression);
    Object lookUpVariable(const Token& name, const Variable& expr);

    void operator()(const ExpressionStatement& expressionStatement);
    void operator()(const IfStatement& ifStatement);
    void operator()(const WhileStatement& whileStatement);
    void operator()(const FunctionStatement& functionStatement);

    void operator()(const PrintStatement& printStatement);
    void operator()(const VariableStatement& variableStatement);
    void operator()(const ReturnStatement& returnStatement);
    void operator()(const ClassStatement& returnStatement);

    void operator()(const VoidType*) {
    }

    void operator()(const BlockStatement& blockStatement);
    void executeBlock(const std::vector<Statement>& statements,
                      Environment* newEnvironment);

    Object operator()(const Assign& assign);
    Object operator()(const Binary& binary);
    Object operator()(const BinaryAdd& binary);
    Object operator()(const BinarySub& binary);
    Object operator()(const Literal& literal);
    Object operator()(const Grouping& grouping);
    Object operator()(const Unary& unary);
    Object operator()(const Variable& variable);
    Object operator()(const Logical& logical);
    Object operator()(const Call& call);
    Object operator()(const Increment& inc);
    Object operator()(const Decrement& inc);
    Object operator()(const Get& get);
    Object operator()(const Set& set);

    Object operator()(const void*) {
        return nullptr;
    }

    bool isTruthy(const Object& object);
    bool isEqual(const Object& a, const Object& b);
    // we could rely on the bad_variant_access but this way we throw based
    // on type. might be slower. worth investigating in future.
    void checkNumberOperand(const Token& token, const Object& operand);
    // version of the fuction for binary operators
    void checkNumberOperands(const Token& token, const Object& left,
                             const Object& right);
    std::string stringify(const Object& object);

    // std::shared_ptr<Environment>
    //     globals; // place to store global native functions etc

    // std::shared_ptr<Environment>
    //     environment; // this maybe overriden temporarily by blocks and then
    //                  // set

    // set
    // back
    bool enableEnvironmentSwitching =
        false; // when looping, we dont need to push and pop environments so
               // we disable

    // struct ExprComparitor {
    //     bool operator()(const LookupVariableVariant& a,
    //                     const LookupVariableVariant& b) const {
    //         return &a < &b; // compare address of Expressions to see if they
    //                         // are equal?
    //     }
    // };

    struct [[nodiscard]] objVectorHelper {
        std::vector<Object>& objVector;
        size_t index;
    };

    Environment* retrieveEnvironment(Environment* closure = nullptr);
    void clearEnvironmentFromStack(Environment* environment);
    void clearEnvironmentFromStack(int handle);

    [[nodiscard]] objVectorHelper getNewArgumentVector();
    void clearArgumentVector(size_t index);

    Object currentReturn = Object{nullptr};
    bool containsReturn = false;
    bool finishing = false;
    std::shared_ptr<Environment>
        globalsHold; // place to store global native functions etc

    ObjectAdder adder;
    ObjectSubber subber;


    sparestack<std::vector<Object>> argumentsStack;
    stablestack<std::vector<Object>> argumentsStack2;

    uniquestack<boost::local_shared_ptr<Environment>> Environments;

    Environment* globals; // place to store global native functions etc

    Environment*
        environment; // this maybe overriden temporarily by blocks and then
    // std::unordered_map<LookupVariableVariant, int> locals;
};
} // namespace cpplox