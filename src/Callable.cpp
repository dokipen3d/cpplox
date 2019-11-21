#include "Callable.h"


namespace cpplox {
Object NativeFunction::call(const Interpreter& interpreter,
                            const std::vector<Object> arguments) {
    return {};
}

Object FunctionObject::call(const Interpreter& interpreter,
                            const std::vector<Object> arguments) {
    return {};
}

std::ostream& operator<<(std::ostream& os, const NativeFunction& dt) {
    os << "Native Function"
       << "\n";
}

std::ostream& operator<<(std::ostream& os, const FunctionObject& dt) {
    os << "Function Object"
       << "\n";
}

} // namespace cpplox