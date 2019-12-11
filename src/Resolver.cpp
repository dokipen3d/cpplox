#include "Resolver.h"

#include <variant>

namespace cpplox {

Resolver::Resolver(const Interpreter& interpreter)
    : m_interpreter(interpreter) {
}

void Resolver::resolve(const Statement& statement) {
    std::visit(*this, statement);
}

void Resolver::resolve(const std::vector<Statement>& statements) {
    for (auto& statement : statements) {
        resolve(statement);
    }
}

void Resolver::resolve(const Expr& expr) {
    std::visit(*this, static_cast<ExprVariant>(expr));
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

void Resolver::operator()(const ExpressionStatement& expressionStatement) {
}
void Resolver::operator()(const IfStatement& ifStatement) {
}
void Resolver::operator()(const WhileStatement& whileStatement) {
}
void Resolver::operator()(const FunctionStatement& functionStatement) {
}
void Resolver::operator()(const PrintStatement& printStatement) {
}
void Resolver::operator()(const VariableStatement& variableStatement) {
    declare(variableStatement.name);
    if (variableStatement.initializer != nullptr) {
        resolve(variableStatement.initializer);
    }
    define(variableStatement.name);
}
void Resolver::operator()(const ReturnStatement& variableStatement) {
}

void Resolver::operator()(const BlockStatement& blockStatement) {
    beginScope();
    resolve(blockStatement.statements);
    endScope();
}
void Resolver::operator()(const Assign& assign) {
}
void Resolver::operator()(const Binary& binary) {
}
void Resolver::operator()(const Literal& literal) {
}
void Resolver::operator()(const Grouping& grouping) {
}
void Resolver::operator()(const Unary& unary) {
}
void Resolver::operator()(const Variable& variable) {
}
void Resolver::operator()(const Logical& logical) {
}
void Resolver::operator()(const Call& call) {
}

} // namespace cpplox