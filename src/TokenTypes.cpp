#include "Environment.h"
#include "ExpressionInterpreter.h"
#include "Statement.hpp"

namespace cpplox {

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

std::ostream& operator<<(std::ostream& os,
                         const recursive_wrapper<LoxClass>& loxClass) {
    os << loxClass.t[loxClass.index].name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const LoxClass& loxClass) {
    os << loxClass.name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const LoxInstance& loxInstance) {
    os << loxInstance.klass << ".instance\n";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const recursive_wrapper<LoxInstance>& loxInstance) {
    os << loxInstance.t[loxInstance.index].klass << ".instance\n";
    return os;
}

NativeFunction::NativeFunction(
    std::function<Object(const Interpreter&, const std::vector<Object>)> func,
    std::function<int()> arity)
    : m_func(func), arity(arity) {
}

FunctionObject::FunctionObject(Interpreter* interpreter,
                               const FunctionStatement* functionStatement)
    : interpreter(interpreter), m_declaration(functionStatement),
      closure2(interpreter->Environments[interpreter->environment->handle]) {
    // std::cout << "making FO, getting parent env "
    //           << interpreter->environment->handle << "with uc "
    //           << closure2.local_use_count() << "\n";
}

// FunctionObject::FunctionObject(FunctionObject const& other)
//     : m_declaration(other.m_declaration){
//     std::cout << "copy\n";
//     interpreter = other.interpreter;
//     closure2 = other.closure2;
// }

// FunctionObject& FunctionObject::operator=(const FunctionObject& other)
// {
//          interpreter = other.interpreter;
//     m_declaration = other.m_declaration;
//     //std::cout << "assign\n";

//     closure = other.closure;
//     //this->closure->refCount++;
//     envToClearDelayed = other.envToClearDelayed; // for closure
//     return *this;
// }

std::size_t FunctionObject::arity() const {
    return m_declaration->params.size();
};

FunctionObject::~FunctionObject() {
    // there maybe more than 1 ref to this environment as we may have copied the
    // function object out as a closure we want to still take advangtage of the
    // re-use stack though so we get the handle, relinquish this ptr to it to
    // decrement the count and then call clear with the index to make sure its
    // gets reused.

    // if (!interpreter) {
    // }
    // this prevents double free. for some reason an object is kept around and
    // is valid but has a use count of 0

    // if(closure2->handle == 0){
    //     std::cout << closure2->handle << " "
    //           <<
    //           interpreter->Environments[closure2->handle].local_use_count()
    //           << " going\n";
    //      //std::cout << "going\n";
    //     return;
    // }

    std::size_t index = closure2->handle;

    // if (interpreter->Environments[index].local_use_count() == 0) {
    //     return;
    // }
    // std::cout << closure2->handle << " "
    //           << interpreter->Environments[index].local_use_count()
    //           << " going\n";
    // if (closure2.local_use_count() > 0) {

    // std::cout << " about to reset\n";
    closure2.reset();

    // std::cout << " after to reset\n";

    // } else {
    //     return;
    // }

    // std::cout << "-> " <<  interpreter->Environments[index].local_use_count()
    // << "\n";

    // std::cout << "end\n";

    if (!interpreter->finishing &&
        interpreter->Environments[index].local_use_count() == 1) {

        // std::cout << "after reset " << index << " lc: "
        //           << interpreter->Environments[index].local_use_count() <<
        //           "\n";
        interpreter->clearEnvironmentFromStack(index);
    }
}

Object FunctionObject::operator()(Interpreter& interpreter,
                                  const std::vector<Object>& arguments) {

    // for some reason to get this to work, I had to choose the current env not
    // the global one like in the book. might have something to do with how we
    // set up the globals to just be one level higher than the first env instead
    // of a seperate env

    // auto environment = std::make_shared<Environment>(closure);
    auto environment = interpreter.retrieveEnvironment(closure2.get());
    // sub = environment;
    for (int i = 0; i < m_declaration->params.size(); i++) {
        environment->define(m_declaration->params[i].lexeme, arguments[i]);
    }

    interpreter.executeBlock(m_declaration->body, environment);

    // std::cout << "exec\n";

    if (interpreter.containsReturn) {
        if (!interpreter.currentReturn.is<FunctionObject>()) {
            interpreter.clearEnvironmentFromStack(environment);
        }

        interpreter.containsReturn = false;
        return interpreter.currentReturn;
    }

    return nullptr;
}

const Object& LoxInstance::get(const cpplox::Token& name) const {
    if (const auto search = properties.find(name.lexeme);
        search != properties.end()) {
        return search->second;
    }

    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(const Token& name, const Object& value) {
    properties.insert_or_assign(name.lexeme, value);
}

Object LoxClass::operator()(Interpreter& interpreter,
                            const std::vector<Object>& arguments) {
    return LoxInstance(this);
}

} // namespace cpplox