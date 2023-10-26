#pragma once
#include "Expr.hpp"
#include <sstream>
#include <string>
#include <variant>
//#include "thirdparty/mvariant.hpp"


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
    void operator()(const BinaryAdd& binary);
    void operator()(const BinarySub& binary);

    void operator()(const Literal& literal);
    void operator()(const Grouping& grouping);
    void operator()(const Unary& unary);
    void operator()(const Variable& variable);
    void operator()(const Logical& logical){}
    void operator()(const Call& call){}
    void operator()(const Increment& inc){}
    void operator()(const Decrement& dec){}
    void operator()(const Get& get){}
    void operator()(const Set& set){}





    void operator()(const void*) {
    }
};
} // namespace cpplox