#include "TokenTypes.h"
#include "ExpressionInterpreter.h"

namespace cpplox {

	NativeFunction::NativeFunction(
    std::function<Object(const Interpreter&, const std::vector<Object>)> func,
    std::function<int()> arity)
    : m_func(func), arity(arity) {
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