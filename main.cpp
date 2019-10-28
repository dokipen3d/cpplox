#include "Error.h"
#include "Expr.hpp"
#include "ExpressionInterpreter.h"
#include "ExpressionParser.h"
#include "Scanner.h"
#include "Statement.hpp"
#include "TimeIt.hpp"
#include "TokenTypes.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum class Mode { REPL, FILE };

cpplox::Interpreter interpreter;

void run(const std::string& code, Mode mode) {

    std::cout << "scanning tokens\n";

    auto tokens = cpplox::scanTokens(code);
    // for (auto& t : tokens){
    //         std::cout << t.lexeme << " ";
    // }
    std::cout << "\n";

    std::cout << "parsing expressions\n";
    cpplox::Parser parser(tokens);
    std::vector<cpplox::Statement> statements;
    if (std::count_if(std::cbegin(tokens), std::cend(tokens),
                      [](const cpplox::Token& token) {
                          return token.eTokenType ==
                                 cpplox::ETokenType::SEMICOLON;
                      }) > 0) {
        statements = parser.parse();
    } else if (mode == Mode::FILE) { // there were no semi colons and it is file mode so we shouldnt allow that
        hadError = true;
        break;
    } else if (mode == Mode::REPL) { // we must have a single expression, so we
                                     // parse expression and
                                     // wrap it in a print statement expression
        cpplox::Expr expression = parser.parseExpression();
        statements.emplace_back(cpplox::PrintStatement(expression));
    }
    if (hadError) {
        std::cout << "parse error\n";
        return;
    }

    // parser.print(expression);
    std::cout << "interpreting expression\n";
    try {

        interpreter.interpret(statements);

    } catch (std::exception& error) {
        std::cout << error.what() << "\n";
    }
}

// dummy functions to make main run at this stage
void runFile(const std::string& filePath) {
    std::cout << "running file " << filePath << "\n";
    std::ifstream inputFileStream(filePath);
    std::string code(std::istreambuf_iterator<char>{inputFileStream}, {});
    run(code);
};
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
};

int main(int argumentCount, char* argumentValues[]) {

    // ddcpplox::visit2();

    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        runFile(argumentValues[1]);
        if (hadError) {
            return 1;
        }
        if (hadRuntimeError) {
            return 1;
        }
    } else {
        runPrompt();
    }
}