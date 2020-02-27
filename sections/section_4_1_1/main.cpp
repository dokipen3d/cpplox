#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace cpplox {

std::vector<std::string> scanTokens(const std::string& code) {
    return {"token 1", "token2"};
}

} // end namespace cpplox

struct Lox {

    bool hadError = false;

    void report(int line, const std::string& where,
                const std::string& message) {
        std::cout << "[line " << line << "] Error" << where << ": " << message
                  << "\n";
        hadError = true;
    }

    void run(const std::string& code) {
        auto tokens = cpplox::scanTokens(code);

        // print the tokens
        for (auto& token : tokens) {
            std::cout << token << "\n";
        }
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
        }
        // set error back in case there was an error as we don't want to kill session
        hadError = false;
    };
}; // end of struct lox

int main(int argumentCount, char* argumentValues[]) {

    Lox lox;

    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        lox.runFile(argumentValues[1]);
        if (lox.hadError) {
            return 1;
        }
    } else {
        lox.runPrompt();
    }
}