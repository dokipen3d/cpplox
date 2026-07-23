#include "ExpressionPrinterVisitor.h"
#include <iostream>
#include <string>
#include "Utilities.hpp"
#include <boost/core/demangle.hpp>


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

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
    ast << RED;
    std::visit(*this, static_cast<ExprVariant>(expression));
    ast << RESET;
    //std::cout << ast.str();
}

void ExpressionPrinterVisitor::printSt(const Statement& st) {
    ast << GREEN;

    std::visit(*this, static_cast<StatementVariant>(st));
    ast << RESET;

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

void ExpressionPrinterVisitor::operator()(const Get& get){
 indent("get");
    
    unindent();
}
void ExpressionPrinterVisitor::operator()(const Set& set){
     indent("set");
    
    unindent();
}

void ExpressionPrinterVisitor::parenthesizeStatement(const std::string& name, const Statement& st) {
   
    //std::visit(*this, static_cast<ExprVariant>(exprA));
        std::visit(*this, static_cast<StatementVariant>(st));
}

void ExpressionPrinterVisitor::parenthesize(const std::string& name,
                                            const Expr& expr) {
    parenthesize(name, expr, nullptr);
}

void ExpressionPrinterVisitor::operator()(const Assign& assign) {
    indent("assign: " + assign.name.lexeme);
    std::visit(*this, static_cast<ExprVariant>(assign.value));

    unindent();
}

void ExpressionPrinterVisitor::operator()(const Binary& binary) {
    //parenthesize(binary.op.lexeme, binary.left, binary.right);
    //}
    indent("binary");
    
    unindent();
}
void ExpressionPrinterVisitor::operator()(const Literal& literal) {
    std::string out;
    if (literal.val == nullptr) {
        out = "nil";
    } else {
        out = std::visit(
            [&](auto&& arg) {
                // not a void* so can print
                std::stringstream ss;
                ss << arg <<  " " <<  boost::core::demangle(typeid(arg).name()) << ")";
                return ss.str();
            },
            static_cast<ObjectVariant>(literal.val));
    }
    indent("literal:" + out);

    unindent();
}

void ExpressionPrinterVisitor::operator()(const Call& call){
    indent("call: ");
    std::visit(*this, static_cast<ExprVariant>(call.callee));
    
    //ast << "(call)\n";
    //std::visit(*this, static_cast<ExprVariant>(call.paren.lexeme));
    for (const auto& expr : call.arguments) {
        std::visit(*this, static_cast<ExprVariant>(expr));
    }
    unindent();
}


void ExpressionPrinterVisitor::operator()(const BinaryAdd& binary) {
    indent("BinaryAddExpr");
    parenthesize(binary.op.lexeme, binary.left, binary.right);
    unindent();
    //}
}

void ExpressionPrinterVisitor::operator()(const BinarySub& binary) {
    indent("BinarySubExpr");
    parenthesize(binary.op.lexeme, binary.left, binary.right);
    unindent();
    //}
}

void ExpressionPrinterVisitor::operator()(const Grouping& grouping) {
    parenthesize("group", grouping.expr);
}

void ExpressionPrinterVisitor::operator()(const Variable& variable) {
    indent("variable lookup:" + variable.name.lexeme);
    
    unindent();
    // stub
}
void ExpressionPrinterVisitor::operator()(const Unary& unary) {
    parenthesize(unary.token.lexeme, unary.expr);
}
void ExpressionPrinterVisitor::operator()(const Uninitialized& uninit){
    indent("Expr: uninitialized: ");
    unindent();
}

void ExpressionPrinterVisitor::operator()(const BlockStatement& blockstatement){
    indent("block: ");
    unindent();
}
void ExpressionPrinterVisitor::operator()(const IfStatement& iftatement){
    indent("if: ");
    unindent();
}
void ExpressionPrinterVisitor::operator()(const WhileStatement& whilestatement){
    indent("while: ");
    unindent();
}
void ExpressionPrinterVisitor::operator()(const FunctionStatement& functionstatement){

    std::string s = ast.str();
    std::size_t lastNewlinePos = s.rfind('\n');
    std::size_t spaces = s.size()-lastNewlinePos-1;


    //std::string(tabs*4, ' ') << "(" << "func: " << functionstatement.name.lexeme << "\n";
    //tabs++;
    indent("func: " + functionstatement.name.lexeme + " ret: " +  functionstatement.returnType.lexeme);
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
        std::visit(*this, static_cast<ExprVariant>(printstatement.expression));

        unindent();
    }
void ExpressionPrinterVisitor::operator()(const VariableStatement& variableStatement){

    std::string s = ast.str();
    std::size_t lastNewlinePos = s.rfind('\n');
    std::size_t spaces = s.size()-lastNewlinePos-1;
   
   
   // ast << "(" << "variable: " << variableStatement.name.lexeme << " = ";
    indent("variable: " + variableStatement.name.lexeme + ", type == " + variableStatement.typeName.lexeme);
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
    
    print(returnstatement.value);
    ast << RESET;
    unindent();
}
} // namespace cpplox

