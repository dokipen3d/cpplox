#include "ExpressionInterpreter.h"

#include "Error.h"
#include "Expr.hpp"
#include "RuntimeError.h"
#include "Utilities.hpp"

#include <functional>
#include <iostream>
#include <iterator> //for std::next()
#include <variant>
#include <vector>

namespace cpplox {

void Interpreter::interpret(Expr expression) {
    try {
        Object value = evaluate(expression);
        this->timeIt.time();
        std::cout << "value is: " << stringify(value) << "\n";
    } catch (const RuntimeError& error) {
        std::cout << "Caught Runtime error"
                  << "\n";

        Error::runtimeError(error);
    }
}

Object Interpreter::evaluate(Expr expression) {
    return std::visit(*this, expression);
}

Object Interpreter::operator()(const Binary& binary) {
    Object left = evaluate(binary.expressionA);
    Object right = evaluate(binary.expressionB);

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
            return std::get<std::string>(left) + std::get<std::string>(right);
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
    case ETokenType::BANG_EQUAL: {
        return !isEqual(left, right);
    }
    case ETokenType::EQUAL_EQUAL: {
        return isEqual(left, right);
    }
    }

    // unreachable
    return nullptr;
}
Object Interpreter::operator()(const Literal& literal) {
    return literal.val;
}

Object Interpreter::operator()(const Grouping& grouping) {
    return evaluate(grouping.expr);
}
Object Interpreter::operator()(const Unary& unary) {
    Object returnObject = nullptr;
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
Object Interpreter::operator()(const std::monostate neverCalled) {
    return nullptr;
}
Object Interpreter::operator()(const NoOp& neverCalled) {
    return nullptr;
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