#include "Error.h"
#include "Expr.hpp"
#include "ExpressionInterpreter.h"
#include "ExpressionParser.h"
#include "ExpressionPrinterVisitor.h"
#include "Resolver.h"
#include "Scanner.h"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include "TokenTypes.h"
#include "Utilities.hpp"
#include <algorithm>
#include <csignal>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void my_handler(int s) {
    printf("Caught signal %d\n", s);
    exit(1);
}

struct lox {

    void run(const std::string& code) {

        // std::cout << "scanning tokens\n";

        auto tokens = cpplox::scanTokens(code);
        // for (auto& t : tokens){
        //         std::cout << t.lexeme << " ";
        // }
        // std::cout << "\n";

        // std::cout << "parsing expressions\n";
        cpplox::Parser parser(tokens);
        std::vector<cpplox::Statement> statements;
        {
            TimeIt timer("parser");

            statements = parser.parse();
        }
        // std::cout << "resolving\n";

        cpplox::Resolver resolver;
        resolver.resolve(statements);

        if (hadError) {
            std::cout << "parse error\n";
            return;
        }

        cpplox::safeToReuse = true;

        try {

            // std::cout << "interpreting expression\n";

            interpreter.interpret(statements);

        } catch (std::exception& error) {
            std::cout << error.what() << "\n";
        }
    }

    // dummy functions to make main run at this stage
    void runFile(const std::string& filePath) {
        // std::cout << "running file " << filePath << "\n";
        std::ifstream inputFileStream(filePath);
        std::string code(std::istreambuf_iterator<char>{inputFileStream}, {});
        run(code);
    }

    void runPrompt() {

        std::cout << "Running prompt\n";
        std::string currentLine;
        std::string lastLine;
        int counter = 0;
        bool runLastLine = false;
        bool backspace = false;

        for (;;) {

            std::getline(std::cin, currentLine);

            cpplox::rtrim(currentLine);

            if (!(currentLine.back() == ';')) {
                currentLine += ";";
                // std::cout << "current line is: " << currentLine << "\n";
            }

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

    // struct sigaction sigIntHandler;

    // sigIntHandler.sa_handler = my_handler;
    // sigemptyset(&sigIntHandler.sa_mask);
    // sigIntHandler.sa_flags = 0;

    // sigaction(SIGINT, &sigIntHandler, NULL);
    // std::stringstream iss("1 a 1 b 4 a 4 b 9");
    // std::cin
    // std::string currentLine;
    // std::getline(std::cin, currentLine, '\n');

    std::cout.precision(18);
    lox l;

    if (argumentCount > 3) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 3) {
        if (strcmp(argumentValues[1], "-p") == 0) {
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