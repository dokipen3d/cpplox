# Scanning


## Fleshing out the apllication 4.1

So here we will be fleshing out main.cpp and creating a basic framework to start our interpreter from. This follows the https://craftinginterpreters.com/scanning.html page closely.

### Main


```c++
int main(int argumentCount, char* argumentValues[]) {

    Lox lox;

    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        lox.runFile(argumentValues[1]);
    } else {
        lox.runPrompt();
    }
}
```
First we create a `Lox` obejct. We will get to creating that soon. it is responsible to reading our code and processing it.

Here we are taking in arguments from the command line and we get passed how many arguments there were and also the arguments themselves as an array of strings (which in c/c++ are pointer to characters ie. char *).

Since the first argument is always the program itself, the second argumen is potentially the name of a scipt file that we want to run.
If there are more than two arguments we inform the user how to use the program correctly.

If there are exactly 2 arguments, it means that the user has given a filename and then we call the `lox.runFile()` function. If there is no argument, then we run the live prompt mode with `lox/runPrompt()`.

### Creating the lox class

There is a slight difference in the way that java and c++ define their classes an main function. In the Java code, main is part of the lox class, whereas in c++ I decided to have a lox class that contained the run(), runFile() and runPrompt() function as members, and then the main function creates a lox object and uses that. One of the reasons for this was to get around some c++ peculiarities (something to do with ODR and etc, I can't quite remember).

```c++
struct Lox {

    // all the following code from run(), runPrompt() and runFile() goes in here

}
```

### Reading a file

Required includes....

```c++
#include <fstream>
#include <string>
```
...

```c++
// in struct Lox { 
void runFile(const std::string& filePath) {
    std::ifstream inputFileStream(filePath);
    std::string code(std::istreambuf_iterator<char>{inputFileStream}, {});
    run(code);
};
```

Here we take in the filePath (c++ takes care of the conversion from char* to std::string) and construct a std::ifstream object which is an input stream. 
Then we construct a std::string directly using the string constructor that takes an iterator. In this case an input stream iterator that has the stream passed into that. This might nit be the most efficient way to read a file, but it is the simplest, and for this application, the speed was not an issue for me.

We then call the run() function which we will get to soon.

### Reading from the command line

Required includes....

```c++
#include <iostream>
```
...

```c++
// in struct Lox { 
void runPrompt() {
    std::cout << "Running prompt\n";
    std::string currentLine;
    for (;;) {
        std::getline(std::cin, currentLine);
        run(currentLine);
    }
};
```

This is run in the case that we want to run interactively and want to process one line at a time. We have a std::string, and then in an infinite loop, call std::getline (passing in the character input stream object std::cin and the string we want to read the code into). Then we pass the code to the run() function.

### Running the code.

Required includes....


```c++
#include <vector>
```
...

```c++
// in struct Lox { 
void run(const std::string& code) {
    auto tokens = cpplox::scanTokens(code);

    //print the tokens
    for (auto& token : tokens){
        std::cout << token << "\n"; 
    }
}
```
Here we take the code, and pass it into the scanToken function (which we will write soon). Then at this point, we just print out the tokens.

This devaties slightly from the java based version, where Scanner is a separate class. I originally had it working like that as well in c++, but I realised that I didn't need the scanner to keep any state around. It's job was to just process the tokens and give them back. So I simple made a scanTokens() free function (which we will move into a sepeate file soon, for now I keep it local in main.cpp to keep the project setup simple).

Also instead of a Java list of tokens, we get back a `std::vector<std::string>` of tokens.

### Stubbing out a temporary dummy scanTokens() function

Here in main.cpp, we will add a cpplox::scanToken() function so that our code will compile at this stage and actually print out the tokens to make sure that everything is working at this stage. This is also the first example of the cpplox namspace that we will be putting most of our classes and types in.  Again, this function will be factored out into a separate file soon but for now we can compile this.

```c++
// above struct Lox {

namespace cpplox {

std::vector<std::string> scanTokens(const std::string& code) {
    return {"token 1", "token2"};
}

} // end namespace cpplox
```
Now we can build our first main.cpp file and run it in the command line!

## Handling Errors 4.1.1

Now we can add some basic error handling into the Lox class. We will eventually factor out error handling into a different file, but again, for this part while we are getting our feet wet, we will do in directly in this file.

```c++
                                                                        /*
struct Lox {                                                            */

    bool hadError = false;
                                                                        /*
    void run(const std::string& code) {                                 */
```

We introduce a new variable within the Lox class to track if we had an Error or not.

Next we add into Lox the report() function.

```c++
                                                                        /*
bool hadError = false;                                                  */

void report(int line, const std::string& where,
            const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
                << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}
                                                                /*
void run(const std::string& code) {                             */
```

In runPrompt()....

```c++
                                                                /*
        run(currentLine);
    }                               */                                                                 
    // set error back in case there was an error as we don't 
    // want to kill session
    hadError = false;                                        /*
};                                                          */
```
In main()...

```c++
                                                            /*
} else if (argumentCount == 2) {
    runFile(argumentValues[1]);                             */
    if (hadError) {
        return 1;
    }                                                       /*
}                                                           */
```

Now we have the ability to report errors. We aren't actually calling that yet anywhere but we will soon!

