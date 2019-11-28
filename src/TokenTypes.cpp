#include "ExpressionInterpreter.h"
#include "Statement.hpp"


namespace cpplox {

NativeFunction::NativeFunction(
    std::function<Object(const Interpreter&, const std::vector<Object>)> func,
    std::function<int()> arity)
    : m_func(func), arity(arity) {
}

FunctionObject::FunctionObject(FunctionStatement& functionStatement)
    : m_declaration(functionStatement) {
}

int FunctionObject::arity() {
    return m_declaration.params.size();
};

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