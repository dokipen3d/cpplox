#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TokenTypes.h"
#include "Scanner.h"
#include "Error.h"

struct Lox {

    void run(const std::string& code) {
        auto tokens = cpplox::scanTokens(code);
    }

    void runFile(const std::string& filePath) {
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
            // set error back in case there was an error as we don't want to kill session
            hadError = false;
        }

    };
}; // end of struct lox

int main(int argumentCount, char* argumentValues[]) {

    Lox lox;

    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        lox.runFile(argumentValues[1]);
        if (hadError) {
            return 1;
        }
    } else {
        lox.runPrompt();
    }
}