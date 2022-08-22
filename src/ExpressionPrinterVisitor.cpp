#include "ExpressionPrinterVisitor.h"
#include <iostream>
#include <string>

namespace cpplox {

void ExpressionPrinterVisitor::print(const Expr& expression) {
    mpark::visit(*this, static_cast<ExprVariant>(expression));
    std::cout << ast.str();
}

void ExpressionPrinterVisitor::parenthesize(const std::string& name,
                                            const Expr& exprA,
                                            const Expr& exprB) {
    ast << "(" << name;

    ast << " ";
    mpark::visit(*this, static_cast<ExprVariant>(exprA));
    ast << " ";
    mpark::visit(*this, static_cast<ExprVariant>(exprB));

    ast << ")";
}

void ExpressionPrinterVisitor::parenthesize(const std::string& name,
                                            const Expr& expr) {
    parenthesize(name, expr, nullptr);
}

void ExpressionPrinterVisitor::operator()(const Assign&) {
}

void ExpressionPrinterVisitor::operator()(const Binary& binary) {
    parenthesize(binary.op.lexeme, binary.left, binary.right);
    //}
}
void ExpressionPrinterVisitor::operator()(const Literal& literal) {
    if (literal.val == nullptr) {
        ast << "nil";
    } else {
        mpark::visit(
            [&](auto&& arg) {
                // not a void* so can print
                ast << arg;
            },
            static_cast<ObjectVariant>(literal.val));
    }
}

void ExpressionPrinterVisitor::operator()(const Grouping& grouping) {
    parenthesize("group", grouping.expr);
}

void ExpressionPrinterVisitor::operator()(const Variable&) {
    // stub
}
void ExpressionPrinterVisitor::operator()(const Unary& unary) {
    parenthesize(unary.token.lexeme, unary.expr);
}

} // namespace cpplox