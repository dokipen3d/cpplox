#include <fstream>
#include <iostream>
#include <string>
#include <vector>

bool hadError = false;

class Scanner {
  public:
    Scanner(std::string sourceIn) : source(std::move(sourceIn)) {
    }
    std::vector<std::string> scanTokens() {
        return {"token1", "token2"};
    }

  private:
    std::string source;
};

void run(const std::string& code) {
    Scanner scanner(code);
    auto tokens = scanner.scanTokens();

    // print the tokens
    for (auto& token : tokens) {
        std::cout << token << "\n";
    }
}

void report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}



// dummy functions to make main run at this stage
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

int main(int argumentCount, char* argumentValues[]) {

    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        runFile(argumentValues[1]);
        if (hadError) {
            return 1;
        }
    } else {
        runPrompt();
    }
}
