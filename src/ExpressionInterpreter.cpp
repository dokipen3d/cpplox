#include "ExpressionInterpreter.h"
#include "Error.h"
#include "ExceptionError.h"
#include "Utilities.hpp"
//#include "Object.h"
#include "TokenTypes.h"
#include "thirdparty/visit.hpp"

#include <functional>
#include <iostream>
#include <iterator> //for std::next()
#include <vector>

namespace cpplox {

void Interpreter::interpret(const std::vector<Statement>& statements) {
    try {
        TimeIt timer("interpreter");
        // std::cout << "size of object = " << sizeof(Object) << " bytes.\n";
        // std::cout << "size of expr = " << sizeof(Expr) << " bytes.\n";

        for (const auto& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError& error) {
        std::cout << "Caught Runtime error"
                  << "\n";

        Error::runtimeError(error);
    }
}

void Interpreter::execute(const Statement& statementToExecute) {
    rollbear::visit(*this, statementToExecute);
}

Object Interpreter::evaluate(const Expr& expression) {
    return rollbear::visit(*this, static_cast<ExprVariant>(expression));
}

Object Interpreter::lookUpVariable(const Token& name, const Variable& expr) {
    if (expr.distance != -1) {
        // inc ref count of environment being looked up
        // Environment* anc = environment->ancestor(expr.distance);
        // anc->refCount++;
        return environment->getAt(expr.distance, name.lexeme);
    } else {
        return globals->get(name);
    }
}

void Interpreter::operator()(const ExpressionStatement& expressionStatement) {
    evaluate(expressionStatement.expression);
    return;
}

void Interpreter::operator()(const IfStatement& ifStatement) {
    if (isTruthy(evaluate(ifStatement.condition))) {
        execute(ifStatement.thenBranch);
    } else if (ifStatement.elseBranch != nullptr) {
        execute(ifStatement.elseBranch);
    }
}

void Interpreter::operator()(const WhileStatement& whileStatement) {
    enableEnvironmentSwitching = false;
    while (isTruthy(evaluate(whileStatement.condition))) {
        execute(whileStatement.body);
    }
    enableEnvironmentSwitching = true;
}

void Interpreter::operator()(const PrintStatement& printStatement) {
    Object value = evaluate(printStatement.expression);
    std::cout << std::fixed << stringify(value) << "\n";
    return;
}

void Interpreter::operator()(const VariableStatement& variableStatement) {
    Object value = nullptr;
    if (variableStatement.initializer != nullptr) {
        value = evaluate(variableStatement.initializer);
    }

    environment->define(variableStatement.name.lexeme, value);
    return;
}

void Interpreter::operator()(const ReturnStatement& returnStatement) {
    // Object value;
    if (returnStatement.value != nullptr) {
        // value = evaluate(returnStatement.value);
        //  throw Return(value);
        // Object value = evaluate(returnStatement.value);
        currentReturn = {evaluate(returnStatement.value)};
        containsReturn = true;
    }
}

void Interpreter::operator()(const FunctionStatement& functionStatement) {
    // Each function call gets its own environment. Otherwise, recursion would
    // break. If there are multiple calls to the same function in play at the
    // same time, each needs its own environment, even though they are all calls
    // to the same function.
    // Here we are taking a function syntax node (a compile time representation)
    // and converting it to its runtime representation
    // const FunctionObject functionObject(&functionStatement,
    // this->environment);
    const Object& functionObject =
        FunctionObject(this, &functionStatement, this->environment);
    environment->define(functionStatement.name.lexeme, functionObject);
}

plf::colony<Environment>::iterator
Interpreter::getNewEnvironment(Environment* closure) {
    // Environment ev(closure);
    // auto index = Environments.push(
    //     Environment(closure), [&](auto index, auto& environment) {
    //     std::cout << "setting env to " << index << "\n";
    //     environment.handle = index;
    //     environment.values.clear();
    // });

    auto it = EnvironmentsColony.insert(Environment(closure));
    // it->it = it;

    return it;
}

Environment* Interpreter::retrieveEnvironment(Environment* closure) {

    auto index = Environments.retrieve([&](auto index, auto& env) {
        env->enclosing = closure;
        env->handle = index;
        env->values.clear();
    });

    return Environments[index].get();
}

void Interpreter::clearEnvironmentFromStack(size_t index) {
    Environments.eraseAt(index);
}

void Interpreter::ClearEnvironment(Environment* environment) {
    // environment->values.clear();
    // Environments.eraseAt(environment->handle);

    // delete self
    // EnvironmentsColony.erase(environment->it);
}

void Interpreter::operator()(const BlockStatement& blockStatement) {

    // auto newEnvironement = std::make_shared<Environment>(environment);
    // auto newEnvironmentPtr = Interpreter::getNewEnvironment(environment);
    auto newEnvironmentPtr = retrieveEnvironment(environment);

    executeBlock(blockStatement.statements,
                 newEnvironmentPtr); // pass in current env as parent

    // clean up env by marking it as free in the storage
    // ClearEnvironment(newEnvironmentPtr);
    // EnvironmentsColony.erase(newEnvironmentPtr);
    clearEnvironmentFromStack(newEnvironmentPtr->handle);
    return;
}

// this is also called by function objects, so the env might not be the one
// created by operator()(BlockStatement) above
void Interpreter::executeBlock(const std::vector<Statement>& statements,
                               Environment* newEnvironment) {

    if (true) {
        // this stack will take ownership
        const auto previous = environment;
        auto final = finally([&]() { this->environment = previous; });

        // main root will get a new one which stores a raw to prev itself. prev
        // will not be moved as it remains on this stack. so pointer should
        // still stay valid.
        // 2. if this is called by  operator()(BlockStatement) then the
        // enclosing of newEnv will be previous
        this->environment = newEnvironment;

        // these will go and possibly make env be moved/chagned but thats okay,
        // they will be taken over by lower stacks
        for (const auto& statement : statements) {
            execute(statement);
            // if we have encountered a return in this block then don't execute
            // any further statemenets (there could be multiple returns)
            if (containsReturn) {
                return;
            }
        }
        // destructor of the environment argument will reset the top level
        // interpreter env to its current parent
        // this->environment = std::move(previous);
    } else {
        enableEnvironmentSwitching = true;
        for (auto& statement : statements) {
            execute(statement);
            if (containsReturn) {
                break;
            }
        }
        enableEnvironmentSwitching = false;
    }
}

Object Interpreter::operator()(const Binary& binary) {
    // Object returnValue;

    Object left = evaluate(binary.left);
    Object right = evaluate(binary.right);

    switch (binary.op.eTokenType) {
    case ETokenType::GREATER: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) > std::get<double>(right);
    }
    case ETokenType::GREATER_EQUAL: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) >= std::get<double>(right);
    }
    case ETokenType::LESS: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) < std::get<double>(right);
    }
    case ETokenType::LESS_EQUAL: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) <= std::get<double>(right);
    }
    case ETokenType::MINUS: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) - std::get<double>(right);
    }
    case ETokenType::PLUS: {
        // this is dynamically checking the type and also making sure both
        // are the same type. if the types are different, what do we do?
        if (left.is<double>() && right.is<double>()) {
            return std::get<double>(left) + std::get<double>(right);
        }
        if (left.is<std::string>() && right.is<std::string>()) {

            return left.get<std::string>() + right.get<std::string>();
            // return
            // static_cast<std::string>(left.get<cpplox::recursive_wrapper<std::string>>())
            // +
            // static_cast<std::string>(left.get<cpplox::recursive_wrapper<std::string>>());
        }
        // this case already has type checking built into which is why it
        // doesnt call checkOperands. intead if we get here, then we throw
        // the error directly which the check would have done
        throw RuntimeError(binary.op, "Operands must be a number.");
    }
    case ETokenType::SLASH: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) / std::get<double>(right);
    }
    case ETokenType::STAR: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) * std::get<double>(right);
    }
    case ETokenType::MOD: {
        checkNumberOperands(binary.op, left, right);
        return std::fmod(std::get<double>(left), std::get<double>(right));
    }
    case ETokenType::BANG_EQUAL: {
        return !isEqual(left, right);
    }
    case ETokenType::EQUAL_EQUAL: {
        return isEqual(left, right);
    }
    }
    return {};
}

Object Interpreter::operator()(const Assign& assign) {
    Object value = evaluate(assign.value);

    // auto search = locals.find(assign);
    // if (search != locals.end()) {
    //     int distance = search->second;
    //     environment->assignAt(distance, assign.name, value);
    // } else {
    //     globals->assign(assign.name, value);
    // }
    if (assign.distance != -1) {
        int offset = enableEnvironmentSwitching == false;
        environment->assignAt(assign.distance, assign.name, value);
    } else {
        globals->assign(assign.name, value);
    }
    return value;
}

Object Interpreter::operator()(const Literal& literal) {
    return literal.val;
}

Object Interpreter::operator()(const Variable& variable) {
    return lookUpVariable(variable.name, variable);
}

Object Interpreter::operator()(const Grouping& grouping) {
    return evaluate(grouping.expr);
}
Object Interpreter::operator()(const Unary& unary) {
    Object right = evaluate(unary.expr);

    switch (unary.token.eTokenType) {
    case ETokenType::MINUS: {
        checkNumberOperand(unary.token, right);

        // extract the value from the Object variant. if it is the wrong
        // type, an exception will get thrown. this is equivalent to the
        // cast in java failing and throwing

        return -std::get<double>(right);
    }
    case ETokenType::BANG: {
        return !isTruthy(right);
    }
    }
    return {};
}

Object Interpreter::operator()(const Logical& logical) {
    Object left = evaluate(logical.left);

    if (logical.op.eTokenType == ETokenType::OR) {
        if (isTruthy(left)) {
            return left;
        }
    } else {
        if (!isTruthy(left)) {
            return left;
        }
    }

    return evaluate(logical.right);
}

Interpreter::objVectorHelper Interpreter::getNewArgumentVector() {

    auto sizet = argumentsStack.retrieve(
        [](auto index, auto& objVector) { objVector.clear(); });
    return {argumentsStack[sizet], sizet};
}

void Interpreter::clearArgumentVector(size_t index) {
    argumentsStack.eraseAt(index);
}

Object Interpreter::operator()(const Call& call) {
    Object callee = evaluate(call.callee);

    // std::vector<Object> arguments;

    auto objHelper = getNewArgumentVector();
    auto& arguments = objHelper.objVector;
    arguments.reserve(call.arguments.size());

    for (auto& argument : call.arguments) {
        arguments.emplace_back(evaluate(argument));
    }

    // when we visit the callee (which should be a function object ie with a
    // callable member ) we want to call this if it is one of the other
    // types
    auto throwIfWrongType = [&]() -> Object {
        throw RuntimeError(call.paren, "Can only call functions and classes");
    };

    // this is a lambda which will also be called when we visit to prevent
    // the need writing this twice
    auto checkArityAndCallFunction = [&](auto func) -> Object {
        if (arguments.size() != func.arity()) {
            std::stringstream stream;
            stream << "Expected " << func.arity() << " arguments but got "
                   << arguments.size() << ".";
            throw RuntimeError(call.paren, stream.str());
        }

        return func(*this, arguments);
    };
    // clang-format off
    Object ret = rollbear::visit(
        overloaded{[&](const NativeFunction& func) -> Object {
                       return checkArityAndCallFunction(func);
                   },
                   [&](const FunctionObject& func) -> Object {
                       return checkArityAndCallFunction(func);
                   },
                   [&](const bool b) -> Object { throwIfWrongType(); return {};},
                   [&](const std::string s) -> Object { throwIfWrongType(); return {}; },
                   [&](const double d) -> Object { throwIfWrongType();  return {};},
                   [&](const void* vs) -> Object { throwIfWrongType();  return {};}},
                static_cast<ObjectVariant>(callee));
    // clang-format on
    clearArgumentVector(objHelper.index);
    return ret;
}

bool Interpreter::isTruthy(const Object& object) {
    if (object.is<bool>()) {
        return std::get<bool>(object);
    }
    if (object == nullptr) {
        return false;
    }

    // fallback case, should be unreachablea
    return true;
}
bool Interpreter::isEqual(const Object& a, const Object& b) {
    // nil is only equal to nil
    // if (a.is<double>() && b.is<double>()) {
    //     return std::abs(std::get<double>(a) - std::get<double>(b)) <
    //            std::numeric_limits<double>::epsilon();
    // }
    if (a.is<double>() && b.is<double>()) {
        return std::get<double>(a) == std::get<double>(b);
    }
    if (a.is<std::string>() && b.is<std::string>()) {
        return a.get<std::string>() == b.get<std::string>();
    }
    return a == b;
}
// we could rely on the bad_variant_access but this way we throw based on
// type. might be slower. worth investigating in future.
void Interpreter::checkNumberOperand(const Token& token,
                                     const Object& operand) {
    if (operand.is<double>()) {
        return;
    }
    throw RuntimeError(token, "Operand must be a number.");
}
// version of the fuction for binary operators
void Interpreter::checkNumberOperands(const Token& token, const Object& left,
                                      const Object& right) {
    if (left.is<double>() && right.is<double>()) {
        return;
    }
    throw RuntimeError(token, "Operands must be a number.");
}
std::string Interpreter::stringify(const Object& object) {
    std::string text; // for rvo
    if (object == nullptr) {
        text = "nil";
    }

    if (object.is<double>()) {
        text = std::to_string(std::get<double>(object));
        stripZerosFromString(text);
    }
    if (object.is<bool>()) {
        text = std::get<bool>(object) ? "true" : "false";
    }

    // must be a string.
    if (object.is<std::string>()) {
        text = object.get<std::string>();
    }
    return text;
}

} // namespace cpplox