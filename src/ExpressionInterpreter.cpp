#include "ExpressionInterpreter.h"
#include "Error.h"
#include "ExceptionError.h"
#include "Utilities.hpp"
//#include "Object.h"
#include "TokenTypes.h"
#include "thirdparty/visit.hpp"
#include "thirdparty/visit2.hpp"
#include <functional>
#include <iostream>
#include <sstream>

#include <iterator> //for std::next()
#include <vector>

namespace cpplox {

void Interpreter::interpret(const std::vector<Statement>& statements) {

    try {

        std::cout << "size of object = " << sizeof(Object) << " bytes.\n";
        std::cout << "size of expr = " << sizeof(Expr) << " bytes.\n";
        std::cout << "size of st = " << sizeof(Statement) << " bytes.\n";
        std::cout << "size of Token = " << sizeof(Token) << " bytes.\n";
        std::cout << "size of Literal = " << sizeof(Literal) << " bytes.\n";
        std::cout << "size of grouping = " << sizeof(Grouping) << " bytes.\n";

        // TimeIt timer("interpreter");

        for (const auto& statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError& error) {
        std::cout << "Caught Runtime error"
                  << "\n";

        Error::runtimeError(error);
    }
    finishing = true;
}

void Interpreter::execute(const Statement& statementToExecute) {
    std::visit(*this, statementToExecute);
}

Object Interpreter::evaluate(const Expr& expression) {
#if (_MSC_VER)
    return cpplox::visit(*this, static_cast<const ExprVariant&>(expression));
#else
    return std::visit(*this, static_cast<const ExprVariant&>(expression));
#endif

    // return [&]() -> Object {
    // const auto index = static_cast<const ExprVariant&>(expression).index();
    // switch (index) {
    // case 0: {
    //     return (*this)(expression.get<Assign>());
    // }
    // case 1: {
    //     return (*this)(expression.get<Binary>());
    // }
    // case 2: {
    //     return (*this)(expression.get<Grouping>());
    // }
    // case 3: {
    //     return (*this)(expression.get<Literal>());
    // }
    // case 4: {
    //     return (*this)(expression.get<Unary>());
    // }
    // case 5: {
    //     return (*this)(expression.get<Variable>());
    // }
    // case 6: {
    //     return (*this)(expression.get<Logical>());
    // }
    // case 7: {
    //     return (*this)(expression.get<Call>());
    // }
    // default: {
    //     return {};
    // }
    // }
    //}();
    // if (expression.is<Call>()) {
    //     return (*this)(expression.get<Call>());
    // } else if (expression.is<Assign>()) {
    //     return (*this)(expression.get<Assign>());
    // } else if (expression.is<Binary>()) {
    //     return (*this)(expression.get<Binary>());
    // } else if (expression.is<Grouping>()) {
    //     return (*this)(expression.get<Grouping>());
    // } else if (expression.is<Variable>()) {
    //     return (*this)(expression.get<Variable>());
    // } else if (expression.is<Unary>()) {
    //     return (*this)(expression.get<Unary>());
    // } else if (expression.is<Literal>()) {
    //     return (*this)(expression.get<Literal>());
    // } else if (expression.is<Logical>()) {
    //     return (*this)(expression.get<Logical>());
    // } else {
    //     return {};
    // }
    //}();
}

Object Interpreter::lookUpVariable(const Token& name, const Variable& expr) {
    if (expr.distance != -1) {
        // inc ref count of environment being looked up
        // Environment* anc = environment->ancestor(expr.distance);
        // anc->refCount++;
        return environment->getAt(expr.distance, name);
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
    // enableEnvironmentSwitching = false;
    while (isTruthy(evaluate(whileStatement.condition)) && !containsReturn) {
        execute(whileStatement.body);
    }
    // enableEnvironmentSwitching = true;
}

void Interpreter::operator()(const PrintStatement& printStatement) {
    const Object& value = evaluate(printStatement.expression);
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
        currentReturn = evaluate(returnStatement.value);
        containsReturn = true;
    }
}

void Interpreter::operator()(const FunctionStatement& functionStatement) {
    // Each function call gets its own environment. Otherwise, recursion
    // would break. If there are multiple calls to the same function in play
    // at the same time, each needs its own environment, even though they
    // are all calls to the same function. Here we are taking a function
    // syntax node (a compile time representation) and converting it to its
    // runtime representation const FunctionObject
    // functionObject(&functionStatement, this->environment);
    // Object functionObject = FunctionObject(this, &functionStatement);
    // std::cout << "start of F block " << this->environment->handle << "\n";

    environment->defineVal(functionStatement.name.lexeme,
                           FunctionObject(this, &functionStatement));
    // std::cout << "end of F block " << this->environment->handle << "\n";
}

Environment* Interpreter::retrieveEnvironment(Environment* closure) {

    Environment* set;
    const auto index = Environments.retrieve([&](auto index, auto& env) {
        if (closure) {
            env->enclosing = Environments[closure->handle];
        }
        env->handle = index;
        env->values.clear();
        set = env.get();
    });
    // std::cout << "got " << index << std::endl;

    return set;
}

void Interpreter::clearEnvironmentFromStack(Environment* environment) {
    // only clear if the count is 1 (ie in the sparestack)
    // if the count is higher, then it means a function object is holding on
    // to it and will clear it eventually in its destructor
    // if (Environments[environment->handle].local_use_count() == 1) {
    // std::cout << "erasing1 " << environment->handle << "\n";
    // environment->values.clear();
    Environments.eraseAt(environment->handle);
    //}
}

void Interpreter::clearEnvironmentFromStack(int handle) {
    // only clear if the count is 1 (ie in the sparestack)
    // if the count is higher, then it means a function object is holding on
    // to it and will clear it eventually in its destructor
    // if (Environments[handle].local_use_count() == 1) {
    // std::cout << "erasing2 " << environment->handle << "\n";
    // Environments[handle]->values.clear();
    Environments.eraseAt(handle);
    //}
}

void Interpreter::operator()(const BlockStatement& blockStatement) {

    auto newEnvironmentPtr = retrieveEnvironment(environment);

    executeBlock(blockStatement.statements,
                 newEnvironmentPtr); // pass in current env as parent

    // clean up env by marking it as free in the storage
    // std::cout << "cleaning env at end of block " << newEnvironmentPtr->handle
    // << "\n";
    clearEnvironmentFromStack(newEnvironmentPtr);
    return;
}

// this is also called by function objects, so the env might not be the one
// created by operator()(BlockStatement) above
void Interpreter::executeBlock(const std::vector<Statement>& statements,
                               Environment* newEnvironment) {

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
}

Object Interpreter::operator()(const Binary& binary) {
    // Object returnValue;

    const Object& left = evaluate(binary.left);
    const Object& right = evaluate(binary.right);

    switch (binary.op.eTokenType) {
    case ETokenType::PLUS: {
        // this is dynamically checking the type and also making sure both
        // are the same type. if the types are different, what do we do?
        if (left.is<double>() && right.is<double>()) {
            // return std::get<double>(left) + std::get<double>(right);
            return *left.get_if<double>() + *right.get_if<double>();
        }
        // const auto* a = left.get_if<double>();
        // const auto* b = right.get_if<double>();
        // if(a && b){
        //     return *a + *b;
        // }

        if (left.is<std::string>() && right.is<std::string>()) {

            return left.get<std::string>() + right.get<std::string>();
            // return *left.get_if<std::string>() +
            // *right.get_if<std::string>();
            //  return
            //  static_cast<std::string>(left.get<cpplox::recursive_wrapper<std::string>>())
            //  +
            //  static_cast<std::string>(left.get<cpplox::recursive_wrapper<std::string>>());
        }
        // this case already has type checking built into which is why it
        // doesnt call checkOperands. intead if we get here, then we throw
        // the error directly which the check would have done
        throw RuntimeError(binary.op, "Operands must be a number.");
    }
    case ETokenType::MINUS: {
        checkNumberOperands(binary.op, left, right);
        // return std::get<double>(left) - std::get<double>(right);
        return *left.get_if<double>() - *right.get_if<double>();
    }
    case ETokenType::STAR: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) * std::get<double>(right);
    }
    case ETokenType::SLASH: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) / std::get<double>(right);
    }
    case ETokenType::EQUAL_EQUAL: {
        return isEqual(left, right);
    }
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
        // return std::get<double>(left) < std::get<double>(right);
        return *left.get_if<double>() < *right.get_if<double>();
    }
    case ETokenType::LESS_EQUAL: {
        checkNumberOperands(binary.op, left, right);
        return std::get<double>(left) <= std::get<double>(right);
    }
    case ETokenType::MOD: {
        checkNumberOperands(binary.op, left, right);
        // int a = *left.get_if<double>();
        // int b = *right.get_if<double>();
        // return double(a % b);
        return std::fmod(*left.get_if<double>(), *right.get_if<double>());
    }
    case ETokenType::BANG_EQUAL: {
        return !isEqual(left, right);
    }
    }
    return {};
}

Object Interpreter::operator()(const Assign& assign) {
    const Object& value = evaluate(assign.value);

    // auto search = locals.find(assign);
    // if (search != locals.end()) {
    //     int distance = search->second;
    //     environment->assignAt(distance, assign.name, value);
    // } else {
    //     globals->assign(assign.name, value);
    // }
    if (assign.distance != -1) {
        // int offset = enableEnvironmentSwitching == false;
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
    const Object& right = evaluate(unary.expr);

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
    const Object& left = evaluate(logical.left);

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

    const auto& objHelper = getNewArgumentVector();
    auto& arguments = objHelper.objVector;
    arguments.reserve(call.arguments.size());

    for (const auto& argument : call.arguments) {
        arguments.emplace_back(evaluate(argument));
    }

    // when we visit the callee (which should be a function object ie with a
    // callable member ) we want to call this if it is one of the other
    // types
    const auto throwIfWrongType = [&]() -> Object {
        throw RuntimeError(call.paren, "Can only call functions and classes");
    };

    // this is a lambda which will also be called when we visit to prevent
    // the need writing this twice
    const auto checkArityAndCallFunction = [&](auto& func) -> Object {
        if (arguments.size() != func.arity()) {
            std::stringstream stream;
            stream << "Expected " << func.arity() << " arguments but got "
                   << arguments.size() << ".";
            throw RuntimeError(call.paren, stream.str());
        }

        return func(*this, arguments);
    };
    // clang-format off
    // const Object ret = std::visit(
    //     overloaded{[&](NativeFunction& func) -> Object {
    //                    return checkArityAndCallFunction(func);
    //                },
    //                [&](FunctionObject& func) -> Object {
    //                    return checkArityAndCallFunction(func);
    //                },
    //                [&](const bool b) -> Object { throwIfWrongType(); return {};},
    //                [&](const std::string& s) -> Object { throwIfWrongType(); return {}; },
    //                [&](const double d) -> Object { throwIfWrongType();  return {};},
    //                [&](const void* vs) -> Object { throwIfWrongType();  return {};}},
    //             static_cast<ObjectVariant>(callee));
    const Object ret = [&]() -> Object {
        if(callee.is<FunctionObject>()){
            //return checkArityAndCallFunction(callee.get<FunctionObject>());
            return checkArityAndCallFunction(static_cast<FunctionObject&>(*callee.get_if<recursive_wrapper<FunctionObject>>()));
        } else if (callee.is<NativeFunction>()){
            return checkArityAndCallFunction(callee.get<NativeFunction>());
        } else {
            throwIfWrongType();  return {};
        }
    }();
    // clang-format on
    clearArgumentVector(objHelper.index);
    return ret;
}

Object Interpreter::operator()(const Increment& inc) {

    Object prev = evaluate(inc.variable);
    checkNumberOperand(inc.op, prev);

    double incLocal = prev.get<double>() + 1;

    const Variable& variableExpr = inc.variable.get<Variable>();

    if (variableExpr.distance != -1) {
        environment->assignAt(variableExpr.distance, variableExpr.name,
                              incLocal);
    }

    else {
        globals->assign(variableExpr.name, incLocal);
    }

    if (inc.type == Increment::Type::POSTFIX) {
        return prev;
    }

    return incLocal;
}

Object Interpreter::operator()(const Decrement& dec) {
    Object prev = evaluate(dec.variable);
    checkNumberOperand(dec.op, prev);

    double decLocal = prev.get<double>() + 1;

    const Variable& variableExpr = dec.variable.get<Variable>();

    if (variableExpr.distance != -1) {
        environment->assignAt(variableExpr.distance, variableExpr.name,
                              decLocal);
    }
    else {
        globals->assign(variableExpr.name, decLocal);
    }

    if (dec.type == Increment::Type::POSTFIX) {
        return prev;
    }

    return decLocal;
}



// LoxObject Interpreter::visit(const DecrementExpr* decrementExpr) {
//     LoxObject prev = interpret(decrementExpr->variable.get());
//     LoxObject dec = prev - LoxObject(1.0);

//     const VariableExpr* variableExpr = decrementExpr->variable.get();
//     if (localsDistances.find(variableExpr) != localsDistances.end()) {
//         environment->assignAt(variableExpr->identifier, dec,
//                               localsDistances[variableExpr]);
//     } else {
//         globalEnv->assign(variableExpr->identifier, dec);
//     }

//     if (decrementExpr->type == DecrementExpr::Type::POSTFIX) {
//         return prev;
//     }
//     return dec;
// }

bool Interpreter::isTruthy(const Object& object) {
    if (object.is<bool>()) {
        return *object.get_if<bool>();
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
        return *a.get_if<double>() == *b.get_if<double>();
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

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6) {
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

std::string Interpreter::stringify(const Object& object) {
    std::string text; // for rvo
    if (object == nullptr) {
        text = "nil";
    }

    if (object.is<double>()) {
        // text = std::to_string(std::get<double>(object));
        text = to_string_with_precision(std::get<double>(object), 11);

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