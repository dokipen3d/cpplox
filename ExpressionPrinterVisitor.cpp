#include "ExpressionPrinterVisitor.h"
#include <iostream>
#include <string>

namespace cpplox {

void ExpressionPrinterVisitor::print(const Expr& expression) {
    std::visit(*this, static_cast<Expr>(expression));
    std::cout << ast.str();
}

void ExpressionPrinterVisitor::parenthesize(const std::string& name,
                                            const Expr& exprA,
                                            const Expr& exprB) {
    ast << "(" << name;

    ast << " ";
    std::visit(*this, static_cast<Expr>(exprA));
    ast << " ";
    std::visit(*this, static_cast<Expr>(exprB));

    ast << ")";
}

void ExpressionPrinterVisitor::parenthesize(const std::string& name,
                                            const Expr& expr) {
    parenthesize(name, expr, nullptr);
}

void ExpressionPrinterVisitor::operator()(const Assign& assign) {
}

void ExpressionPrinterVisitor::operator()(const Binary& binary) {
    parenthesize(binary.op.lexeme, binary.expressionA, binary.expressionB);
    //}
}
void ExpressionPrinterVisitor::operator()(const Literal& literal) {
    if (literal.val == nullptr) {
        ast << "nil";
    } else {
        std::visit(
            [&](auto&& arg) {
                // not a void* so can print
                ast << arg;
            },
            static_cast<variantObject>(literal.val));
    }
}

void ExpressionPrinterVisitor::operator()(const Grouping& grouping) {
    parenthesize("group", grouping.expr);
}

void ExpressionPrinterVisitor::operator()(const Variable& variable) {
    // stub
}
void ExpressionPrinterVisitor::operator()(const Unary& unary) {
    parenthesize(unary.token.lexeme, unary.expr);
}

} // namespace cpplox