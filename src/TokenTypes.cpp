#include "TokenTypes.h"
#include "ExpressionInterpreter.h"


namespace cpplox {

Object NativeFunction::call(const Interpreter& interpreter,
                            const std::vector<Object> arguments) const {
    return {};
}

Object FunctionObject::call(const Interpreter& interpreter,
                            const std::vector<Object> arguments) {
    return {};
}

std::ostream& operator<<(std::ostream& os, const recursive_wrapper<NativeFunction>& dt) {
    os << "Native Function"
       << "\n";
}

std::ostream& operator<<(std::ostream& os, const recursive_wrapper<FunctionObject>& dt) {
    os << "Function Object"
       << "\n";
}
} // namespace cpplox