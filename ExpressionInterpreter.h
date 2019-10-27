#pragma once
#include "Expr.hpp"
#include "TimeIt.hpp"
namespace cpplox {
struct Interpreter {
    void interpret(Expr expression);
    Object evaluate(Expr expression);
    Object operator()(const Binary& binary);
    Object operator()(const Literal& literal);
    Object operator()(const Grouping& grouping);
    Object operator()(const Unary& unary);
    Object operator()(const std::monostate neverCalled);
    Object operator()(const NoOp& neverCalled);
    bool isTruthy(const Object& object);
    bool isEqual(const Object& a, const Object& b);
    // we could rely on the bad_variant_access but this way we throw based on
    // type. might be slower. worth investigating in future.
    void checkNumberOperand(const Token& token, const Object& operand);
    // version of the fuction for binary operators
    void checkNumberOperands(const Token& token, const Object& left, const Object& right);
    std::string stringify(const Object& object);
    const TimeIt timeIt;
};
} // namespace cpplox