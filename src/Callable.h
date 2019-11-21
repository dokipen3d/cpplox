#include <vector>
#include "TokenTypes.h"
#include "ExpressionInterpreter.h"

namespace cpplox {
 

struct Callable {
    virtual Object call(const Interpreter& interpreter, const std::vector<Object> arguments) = 0;
};

}