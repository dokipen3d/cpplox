#include "ExpressionInterpreter.h"
#include "Error.h"
#include "ExceptionError.h"
#include "Expr.hpp"
#include "Utilities.hpp"
//#include "Object.h"
#include "TokenTypes.h"

#include <functional>
#include <iostream>
#include <iterator> //for std::next()
#include <variant>
#include <vector>

namespace cpplox {

void Interpreter::interpret(const std::vector<Statement>& statements) {
    try {
        for (auto& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError& error) {
        std::cout << "Caught Runtime error"
                  << "\n";

        Error::runtimeError(error);
    }
}

void Interpreter::execute(const Statement& statementToExecute) {
    std::visit(*this, statementToExecute);
}

Object Interpreter::evaluate(const Expr& expression) {
    return std::visit(*this, static_cast<ExprVariant>(expression));
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
    std::cout << stringify(value) << "\n";
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

void Interpreter::operator()(const ReturnStatement& variableStatement) {
    Object value;
    if (variableStatement.value != nullptr) {
        value = evaluate(variableStatement.value);
    }

    throw Return(value);
}

void Interpreter::operator()(const FunctionStatement& functionStatement) {
    // Each function call gets its own environment. Otherwise, recursion would
    // break. If there are multiple calls to the same function in play at the
    // same time, each needs its own environment, even though they are all calls
    // to the same function.
    // Here we are taking a function syntax node (a compile time representation)
    // and converting it to its runtime representation
    const FunctionObject functionObject(functionStatement);
    environment->define(functionStatement.name.lexeme, functionObject);
}

void Interpreter::operator()(const BlockStatement& blockStatement) {

    auto newEnvironement = std::make_unique<Environment>(environment.get());
    executeBlock(blockStatement.statements,
                 std::move(newEnvironement)); // pass in current env as parent
    return;
}

// this is also called by function objects, so the env might not be the one
// created by operator()(BlockStatement) above
void Interpreter::executeBlock(const std::vector<Statement>& statements,
                               std::unique_ptr<Environment> newEnvironement) {

    if (enableEnvironmentSwitching) {
        // this stack will take ownership
        auto previous = std::move(environment);

        // main root will get a new one which stores a raw to prev itself. prev
        // will not be moved as it remains on this stack. so pointer should
        // still stay valid.
        // 2. if this is called by  operator()(BlockStatement) then the
        // enclosing of newEnv will be previous
        this->environment = std::move(newEnvironement);

        // these will go and possibly make env be moved/chagned but thats okay,
        // they will be taken over by lower stacks
        for (auto& statement : statements) {
            execute(statement);
        }
        // destructor of the environment argument will reset the top level
        // interpreter env to its current parent
        this->environment = std::move(previous);
    } else {
        enableEnvironmentSwitching = true;
        for (auto& statement : statements) {
            execute(statement);
        }
        enableEnvironmentSwitching = false;
    }
}

Object Interpreter::operator()(const Binary& binary) {
    Object returnValue;

    Object left = evaluate(binary.left);
    Object right = evaluate(binary.right);

    switch (binary.op.eTokenType) {
    case ETokenType::GREATER: {
        checkNumberOperands(binary.op, left, right);
        returnValue = std::get<double>(left) > std::get<double>(right);
        break;
    }
    case ETokenType::GREATER_EQUAL: {
        checkNumberOperands(binary.op, left, right);
        returnValue = std::get<double>(left) >= std::get<double>(right);
        break;
    }
    case ETokenType::LESS: {
        checkNumberOperands(binary.op, left, right);
        returnValue = std::get<double>(left) < std::get<double>(right);
        break;
    }
    case ETokenType::LESS_EQUAL: {
        checkNumberOperands(binary.op, left, right);
        returnValue = std::get<double>(left) <= std::get<double>(right);
        break;
    }
    case ETokenType::MINUS: {
        checkNumberOperands(binary.op, left, right);
        returnValue = std::get<double>(left) - std::get<double>(right);
        break;
    }
    case ETokenType::PLUS: {
        // this is dynamically checking the type and also making sure both
        // are the same type. if the types are different, what do we do?
        if (left.is<double>() && right.is<double>()) {
            returnValue = std::get<double>(left) + std::get<double>(right);
            break;
        }
        if (left.is<std::string>() && right.is<std::string>()) {
            returnValue =
                std::get<std::string>(left) + std::get<std::string>(right);
            break;
        }
        // this case already has type checking built into which is why it
        // doesnt call checkOperands. intead if we get here, then we throw
        // the error directly which the check would have done
        throw RuntimeError(binary.op, "Operands must be a number.");
    }
    case ETokenType::SLASH: {
        checkNumberOperands(binary.op, left, right);
        returnValue = std::get<double>(left) / std::get<double>(right);
        break;
    }
    case ETokenType::STAR: {
        checkNumberOperands(binary.op, left, right);
        returnValue = std::get<double>(left) * std::get<double>(right);
        break;
    }
    case ETokenType::BANG_EQUAL: {
        returnValue = !isEqual(left, right);
        break;
    }
    case ETokenType::EQUAL_EQUAL: {
        returnValue = isEqual(left, right);
        break;
    }
    }

    // unreachable
    return returnValue;
}

Object Interpreter::operator()(const Assign& assign) {
    Object value = evaluate(assign.value);
    environment->assign(assign.name, value);

    return value;
}

Object Interpreter::operator()(const Literal& literal) {
    return literal.val;
}

Object Interpreter::operator()(const Variable& variable) {
    return environment->get(variable.name);
}

Object Interpreter::operator()(const Grouping& grouping) {
    return evaluate(grouping.expr);
}
Object Interpreter::operator()(const Unary& unary) {
    Object returnObject;
    Object right = evaluate(unary.expr);

    switch (unary.token.eTokenType) {
    case ETokenType::MINUS: {
        checkNumberOperand(unary.token, right);

        // extract the value from the Object variant. if it is the wrong
        // type, an exception will get thrown. this is equivalent to the
        // cast in java failing and throwing

        returnObject = -std::get<double>(right);
        break;
    }
    case ETokenType::BANG: {
        returnObject = !isTruthy(right);
        break;
    }
    }
    // unreachable
    return returnObject;
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

Object Interpreter::operator()(const Call& call) {
    Object callee = evaluate(call.callee);

    std::vector<Object> arguments;
    for (auto& argument : call.arguments) {
        arguments.push_back(evaluate(argument));
    }

    // when we visit the callee (which should be a function object ie with a
    // callable member ) we want to call this if it is one of the other types
    auto throwIfWrongType = [&]() -> Object {
        throw RuntimeError(call.paren, "Can only call functions and classes");
    };

    // this is a lambda which will also be called when we visit to prevent the
    // need writing this twice
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
    Object ret = std::visit(
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

    return ret;
}


bool Interpreter::isTruthy(const Object& object) {
    if (object == nullptr) {
        return false;
    }
    if (object.is<bool>()) {
        return std::get<bool>(object);
    }
    // fallback case, should be unreachablea
    return true;
}
bool Interpreter::isEqual(const Object& a, const Object& b) {
    // nil is only equal to nil
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
        text = std::get<std::string>(object);
    }
    return text;
}

} // namespace cpplox