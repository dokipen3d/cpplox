#include "ExpressionPrinterVisitor.h"
#include <iostream>
#include <string>
#include "Utilities.hpp"
#include <boost/core/demangle.hpp>

namespace cpplox {

void ExpressionPrinterVisitor::indent(const std::string& stream){
    ast <<  std::string(tabs*4, ' ') + "<";
    ast << stream << ">\n";// << "\n";
    
    tabs++;
    
    
    
}

void ExpressionPrinterVisitor::unindent(const std::string& stream = ""){
    
    //ast << ">";
    ast << stream;
    
    tabs--;
    //ast << std::string(tabs*4, ' ') ;
    
}

void ExpressionPrinterVisitor::print(const Expr& expression) {
    std::visit(*this, static_cast<ExprVariant>(expression));
    std::cout << ast.str();
}

void ExpressionPrinterVisitor::printSt(const Statement& st) {
    std::visit(*this, static_cast<StatementVariant>(st));
    std::cout << ast.str();
    ast.str(std::string());

}

void ExpressionPrinterVisitor::parenthesize(const std::string& name,
                                            const Expr& exprA,
                                            const Expr& exprB) {
    
    std::visit(*this, static_cast<ExprVariant>(exprA));
        //std::visit(*this, exprA);

    std::visit(*this, static_cast<ExprVariant>(exprB));
    //std::visit(*this, exprB);

}

void ExpressionPrinterVisitor::parenthesizeStatement(const std::string& name, const Statement& st) {
   
    //std::visit(*this, static_cast<ExprVariant>(exprA));
        std::visit(*this, static_cast<StatementVariant>(st));
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
        std::visit(
            [&](auto&& arg) {
                // not a void* so can print
                //ast << "(literal: " << arg << " " << boost::core::demangle(typeid(arg).name()) << ")";
            },
            static_cast<ObjectVariant>(literal.val));
    }
}

void ExpressionPrinterVisitor::operator()(const Call& call){
    indent("call");
    //ast << "(call)\n";
    unindent("");
}


void ExpressionPrinterVisitor::operator()(const BinaryAdd& binary) {
    parenthesize(binary.op.lexeme, binary.left, binary.right);
    //}
}

void ExpressionPrinterVisitor::operator()(const BinarySub& binary) {
    parenthesize(binary.op.lexeme, binary.left, binary.right);
    //}
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
void ExpressionPrinterVisitor::operator()(const Uninitialized& set){
    indent("Expr: uninitialized: ");
    unindent();
}

void ExpressionPrinterVisitor::operator()(const BlockStatement& blockstatement){}
void ExpressionPrinterVisitor::operator()(const IfStatement& iftatement){}
void ExpressionPrinterVisitor::operator()(const WhileStatement& blockstatement){}
void ExpressionPrinterVisitor::operator()(const FunctionStatement& functionstatement){

    std::string s = ast.str();
    std::size_t lastNewlinePos = s.rfind('\n');
    std::size_t spaces = s.size()-lastNewlinePos-1;


    //std::string(tabs*4, ' ') << "(" << "func: " << functionstatement.name.lexeme << "\n";
    //tabs++;
    indent("func: " + functionstatement.name.lexeme);
    //ast << " ";

    for (const cpplox::Statement& statement : functionstatement.body) {
        std::visit(*this, static_cast<StatementVariant>(statement));
    }
    //ast << "\b\b\b\b" << ")" ; 
    //tabs--;
    //ast << std::string(tabs*4, ' ') << "\n)";
    unindent();

}
void ExpressionPrinterVisitor::operator()(const ClassStatement& classtatement){

    std::string s = ast.str();
    std::size_t lastNewlinePos = s.rfind('\n');
    std::size_t spaces = s.size()-lastNewlinePos-1;

    //ast << std::string(spaces+4, ' ') << "(" << "class: " << classtatement.name.lexeme << "\n" << std::string(4, ' ');
    indent("class: " +classtatement.name.lexeme);
    //ast << " ";

    for (const cpplox::Statement& statement : classtatement.functionStatements) {
         std::visit(*this, static_cast<StatementVariant>(statement));
    }

    for (const cpplox::Statement& statement : classtatement.functionProperties) {
        std::visit(*this, static_cast<StatementVariant>(statement));
    }


    //ast << "\b\b\b\b)" ;
    unindent();

}
void ExpressionPrinterVisitor::operator()(const PrintStatement& printstatement){
        indent("print: ");
        unindent();
    }
void ExpressionPrinterVisitor::operator()(const VariableStatement& variableStatement){

    std::string s = ast.str();
    std::size_t lastNewlinePos = s.rfind('\n');
    std::size_t spaces = s.size()-lastNewlinePos-1;
   
   
   // ast << "(" << "variable: " << variableStatement.name.lexeme << " = ";
    indent("variable");
    std::visit(*this, static_cast<ExprVariant>(variableStatement.initializer));
    unindent();
    //ast << ")" << std::string(spaces, ' ');

}
void ExpressionPrinterVisitor::operator()(const ExpressionStatement& expressionstatement){
    indent("expression");

   
    
    std::visit(*this, static_cast<ExprVariant>(expressionstatement.expression));
   
    unindent();
}
void ExpressionPrinterVisitor::operator()(const ReturnStatement& returnstatement){
    std::string s = ast.str();
    std::size_t lastNewlinePos = s.rfind('\n');
    std::size_t spaces = s.size()-lastNewlinePos-1;

    //ast << std::string(tabs*4, ' ') << "(return" << "\n";// << std::string(4, ' ');
    indent("return");
    //ast << std::string(tabs*4, ' ') << ")";
    //ast << "stuff\n";
    unindent();
}
} // namespace cpplox

