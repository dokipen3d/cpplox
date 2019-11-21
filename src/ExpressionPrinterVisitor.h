#pragma once
#include "Expr.hpp"
#include <sstream>
#include <string>
#include <variant>

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

    void operator()(const Assign& assign);
    void operator()(const Binary& binary);
    void operator()(const Literal& literal);
    void operator()(const Grouping& grouping);
    void operator()(const Unary& unary);
    void operator()(const Variable& variable);
    void operator()(const Logical& logical){}
    void operator()(const Call& call){}


    void operator()(const void*) {
    }
};
} // namespace cpplox