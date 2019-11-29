#include "ExpressionInterpreter.h"
#include "Statement.hpp"
#include "Environment.h"


namespace cpplox {

NativeFunction::NativeFunction(
    std::function<Object(const Interpreter&, const std::vector<Object>)> func,
    std::function<int()> arity)
    : m_func(func), arity(arity) {
}

FunctionObject::FunctionObject(const FunctionStatement& functionStatement)
    : m_declaration(functionStatement) {
}

int FunctionObject::arity() {
    return m_declaration.params.size();
};

Object FunctionObject::operator()(Interpreter& interpreter,
                                  const std::vector<Object>& arguments) {

    auto environment = std::make_unique<Environment>(interpreter.globals.get());
    for (int i = 0; i < m_declaration.params.size(); i++){
        environment->define(m_declaration.params[i].lexeme, arguments[i]);

    }
    interpreter.executeBlock(m_declaration.body, std::move(environment));
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