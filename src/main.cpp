#include "Error.h"
#include "Expr.hpp"
#include "ExpressionInterpreter.h"
#include "ExpressionPrinterVisitor.h"
#include "ExpressionParser.h"
#include "Resolver.h"
#include "Scanner.h"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include "TokenTypes.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


struct lox {

    void run(const std::string& code) {

        //std::cout << "scanning tokens\n";

        auto tokens = cpplox::scanTokens(code);
        // for (auto& t : tokens){
        //         std::cout << t.lexeme << " ";
        // }
        //std::cout << "\n";

        //std::cout << "parsing expressions\n";
        cpplox::Parser parser(tokens);
        const std::vector<cpplox::Statement> statements = parser.parse();
        //std::cout << "resolving\n";

        cpplox::Resolver resolver;
        resolver.resolve(statements);

        if (hadError) {
            std::cout << "parse error\n";
            return;
        }

        cpplox::safeToReuse = true;

        try {

            //std::cout << "interpreting expression\n";

            interpreter.interpret(statements);

        } catch (std::exception& error) {
            std::cout << error.what() << "\n";
        }
    }

    // dummy functions to make main run at this stage
    void runFile(const std::string& filePath) {
        //std::cout << "running file " << filePath << "\n";
        std::ifstream inputFileStream(filePath);
        std::string code(std::istreambuf_iterator<char>{inputFileStream}, {});
        run(code);
    }

    void runPrompt() {
        std::cout << "Running prompt\n";
        std::string currentLine;
        for (;;) {
            std::getline(std::cin, currentLine);
            run(currentLine);
            // set error back in case there was an error as we don't want to
            // kill session
            hadError = false;
        }
    }

    cpplox::Interpreter interpreter;
    cpplox::ExpressionPrinterVisitor printer;

};

int main(int argumentCount, char* argumentValues[]) {

    // ddcpplox::visit2();

    std::cout.precision(17);
    lox l;

    if (argumentCount > 3) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 3) {
        if(strcmp(argumentValues[1],"-p") == 0){
            std::cout << "printing AST\n";

        }

    } else if (argumentCount == 2) {
        l.runFile(argumentValues[1]);
        if (hadError) {
            return 1;
        }
        if (hadRuntimeError) {
            return 1;
        }
    } else {
        l.runPrompt();
    }

    std::cout << "done!" << std::endl;
}