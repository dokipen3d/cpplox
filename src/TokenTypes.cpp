#include "Environment.h"
#include "ExpressionInterpreter.h"
#include "Statement.hpp"

namespace cpplox {

NativeFunction::NativeFunction(
    std::function<Object(const Interpreter&, const std::vector<Object>)> func,
    std::function<int()> arity)
    : m_func(func), arity(arity) {
}

FunctionObject::FunctionObject(const FunctionStatement& functionStatement,
                               Environment* closure)
    : m_declaration(functionStatement), closure(closure) {
}

int FunctionObject::arity() const {
    return m_declaration.params.size();
};

Object FunctionObject::operator()(Interpreter& interpreter,
                                  const std::vector<Object>& arguments) {

    // for some reason to get this to work, I had to choose the current env not
    // the global one like in the book. might have something to do with how we
    // set up the globals to just be one level higher than the first env instead
    // of a seperate env

    // auto environment = std::make_shared<Environment>(closure);
    auto environment = interpreter.retrieveEnvironment(closure);

    for (int i = 0; i < m_declaration.params.size(); i++) {
        environment->define(m_declaration.params[i].lexeme, arguments[i]);
    }

    // try {
    interpreter.executeBlock(m_declaration.body, environment);
    //} catch (Return returnValue) { // our custom exception type to embed a
    //value
    // and jump back to here
    //    return returnValue.value;
    interpreter.clearEnvironmentFromStack(
        environment->handle); // <---- this was a big deal. i moved from below
                              // the return value and it was maybe 1.6x faster.
                              // because the env weren't being cleared up!

    if (interpreter.containsReturn) {
        interpreter.containsReturn = false;
        return interpreter.currentReturn.value;
    }
    //}

    return nullptr;
}

std::ostream& operator<<(std::ostream& os,
                         const recursive_wrapper<NativeFunction>& dt) {
    os << "Native Function"
       << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const recursive_wrapper<FunctionObject>& dt) {
    os << "Function Object"
       << "\n";
    return os;
}
} // namespace cpplox