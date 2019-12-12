#include "Resolver.h"
#include "Error.h"
#include "ExpressionInterpreter.h"
#include <variant>

namespace cpplox {

Resolver::Resolver(Interpreter& interpreter) : m_interpreter(interpreter) {
}

void Resolver::resolve(const Statement& statement) {
    std::visit(*this, statement);
}

void Resolver::resolve(const std::vector<Statement>& statements) {
    // for (const auto& statement : statements) {
    //     resolve(statement);
    // }
    for (int i = 0; i < statements.size(); i++) {
        resolve(statements[i]);
    }
}

void Resolver::resolve(const Expr& expr) {
    std::visit(*this, expr);
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) {
        return;
    }

    auto scope = scopes.back();
    scope.insert_or_assign(name.lexeme, false);
}

void Resolver::define(const Token& name) {
    if (scopes.empty()) {
        return;
    }

    scopes.back().insert_or_assign(name.lexeme, true);
}

void Resolver::beginScope() {
    scopes.push_back({});
}

void Resolver::endScope() {
    scopes.pop_back();
}

void Resolver::resolveLocal(const Expr& expr, const Token& name) {
    for (auto i = scopes.size(); i > 0; i--) {
        auto search = scopes[i].find(name.lexeme);
        if (search != scopes[i].end()) {
            m_interpreter.resolve(expr, scopes.size() - i);
            return;
        }
    }
    return;
    // Not found. Assume it is global.
}

void Resolver::resolveFunction(const FunctionStatement& FunctionStatement) {
    beginScope();
    for (auto& param : FunctionStatement.params) {
        declare(param);
        define(param);
    }
    resolve(FunctionStatement.body);
    endScope();
}

void Resolver::operator()(const ExpressionStatement& expressionStatement) {
    resolve(expressionStatement.expression);
}
void Resolver::operator()(const IfStatement& ifStatement) {
    resolve(ifStatement.condition);
    resolve(ifStatement.thenBranch);
    if (ifStatement.elseBranch != nullptr) {
        resolve(ifStatement.elseBranch);
    }
}
void Resolver::operator()(const WhileStatement& whileStatement) {
    resolve(whileStatement.condition);
    resolve(whileStatement.body);
}
void Resolver::operator()(const FunctionStatement& functionStatement) {
    declare(functionStatement.name);
    define(functionStatement.name);

    resolveFunction(functionStatement);
}
void Resolver::operator()(const PrintStatement& printStatement) {
    resolve(printStatement.expression);
}
void Resolver::operator()(const VariableStatement& variableStatement) {
    declare(variableStatement.name);
    if (variableStatement.initializer != nullptr) {
        resolve(variableStatement.initializer);
    }
    define(variableStatement.name);
}
void Resolver::operator()(const ReturnStatement& returnStatement) {
    if (returnStatement.value != nullptr) {
        resolve(returnStatement.value);
    }
}

void Resolver::operator()(const BlockStatement& blockStatement) {
    beginScope();
    resolve(blockStatement.statements);
    endScope();
}
void Resolver::operator()(const Assign& assign) {
    resolve(assign.value);
    resolveLocal(assign, assign.name);
}
void Resolver::operator()(const Binary& binary) {
    resolve(binary.left);
    resolve(binary.right);
}
void Resolver::operator()(const Literal& literal) {
}
void Resolver::operator()(const Grouping& grouping) {
    resolve(grouping.expr);
}
void Resolver::operator()(const Unary& unary) {
    resolve(unary.expr);
}
void Resolver::operator()(const Variable& variable) {
    if (!scopes.empty()) {
        auto varmap = scopes.back();
        auto search = varmap.find(variable.name.lexeme);
        if (search != varmap.end() && search->second == false) {
            cpplox::Error::error(
                variable.name,
                "Cannot read local variable in its own initializer.");
        }
    }
    resolveLocal(variable, variable.name);

    return;
}

void Resolver::operator()(const Logical& logical) {
    resolve(logical.left);
    resolve(logical.right);
}
void Resolver::operator()(const Call& call) {
    resolve(call.callee);

    for (auto& argument : call.arguments) {
        resolve(argument);
    }
}

} // namespace cpplox