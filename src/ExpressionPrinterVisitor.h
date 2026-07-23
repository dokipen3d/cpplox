#pragma once

#include <sstream>
#include <string>
#include <variant>
#include "Statement.hpp"

#include "Expr.hpp"


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
    void printSt(const Statement& st);

    void parenthesize(const std::string& name, const Expr& exprA,
                      const Expr& exprB);
    void parenthesize(const std::string& name, const Expr& expr);
    void parenthesizeStatement(const std::string& name, const Statement& st);


    void operator()(const Assign& assign);
    void operator()(const Binary& binary);
    void operator()(const BinaryAdd& binary);
    void operator()(const BinarySub& binary);
    void operator()(const Literal& literal);
    void operator()(const Grouping& grouping);
    void operator()(const Unary& unary);
    void operator()(const Variable& variable);
    void operator()(const Logical& logical){}
    void operator()(const Call& call);
    void operator()(const Increment& inc){}
    void operator()(const Decrement& dec){}
    void operator()(const Get& get);
    void operator()(const Set& set);
    void operator()(const Uninitialized& set);
    void operator()(const BlockStatement& blockstatement);
    void operator()(const IfStatement& iftatement);
    void operator()(const WhileStatement& blockstatement);
    void operator()(const FunctionStatement& blockstatement);
    void operator()(const ClassStatement& blockstatement);
    void operator()(const PrintStatement& blockstatement);
    void operator()(const VariableStatement& blockstatement);
    void operator()(const ExpressionStatement& blockstatement);
    void operator()(const ReturnStatement& blockstatement);
    void operator()(const VoidType* voidtype){}
    void operator()(const void*) {
    }

    void indent(const std::string& stream);
    void unindent(const std::string& stream);
    int tabs = 0;
};
} // namespace cpplox