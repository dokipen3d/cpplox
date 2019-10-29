#pragma once
#include <string>
#include <variant>
#include <sstream>
#include "Expr.hpp"

// class Expr;
// class Binary;
// class Literal;
// class Grouping;
// class Unary;
// class NoOp;

namespace cpplox {

struct ExpressionPrinterVisitor {

    std::stringstream ast;
    void print(const Expr& expression);
    void parenthesize(const std::string& name, const Expr& exprA,
                      const Expr& exprB);
    void parenthesize(const std::string& name, const Expr& expr);

    void operator()(const Binary& binary);
    void operator()(const Literal& literal);
    void operator()(const Grouping& grouping);
    void operator()(const Unary& unary);
    void operator()(const Variable& variable); 
    void operator()(const std::monostate neverCalled) {
    }
    void operator()(const void* neverCalled) {
    }
    void operator()(const NoOp& neverCalled) {
    }
};
}