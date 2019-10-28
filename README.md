# cpplox
A c++ implementation of the lox interpreter outlined at craftinginterpreters.com


1) create a build folder
2) run 'cmake ..' (by default, it build in debug mode but optionally you can run 'cmake -DCMAKE_BUILD_TYPE=Release' or if on windows, release is specified as a build option shown in step 3 below )
3) run 'cmake --build .' (or on windows ''cmake --build . --config Release')
4) On windows the executable will be '.\Debug\cpplox.exe' if you have built debug or '.\Release\cpplox.exe' for release

#Visual studio 2017 with clang.
- install llvm from the download release page and make sure that LLVM/bin is added to the PATH
- to use clang on windows us this extension...
https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.llvm-toolchain
or choose the LLVM Compiler Toolchain from the extensions in the UI
- then invoke cmake with -T"llvm


Some things that I learned along the way....

- To make a variant recursive, you have to use allocation to be able to use incomplete types (that has the variant itself as a member). This way the storage of the members is on the heap and not inline in the types of the variant. This gets round the issue of types that contain the same types (which will contain the same type) and so on. You can't obviously have infinite sized types.

- the storage for the recursive types is done using a vector in the recursuve wrapper. the naive implementation uses a single std vector for each wrapped type. What we can do instead is make the vector static, define it (so that its in a TU and inline so that all TUs that include the hpp have the same one defined for all THATS WHAT THE INLINE KEYWORD DOES). This makes a static vector for each type (T). This is whole program though so all expressions accross the whole program will use the same one. 
TODOs will be to 1) somehow break that up into more vectors so that its one vec per statement or per TU or something. 2) Clear the vectors when the expressions are no longer needed. 
Also we have dynamically allocated Statements which themselves refer to Expr, so they could potentially provide the storage for the expressions they contain. But there is no easy way for the Expr to know which vector they will be pushing back to. Unless instead of a static vector, there is a static function that tells the 'current' expression where to store its data.  maybe this is overkill.

- const is hard thing to get right, especially when getting conversion functions and  constructors/assingment operators right in wrapper calling visit on a variant. see recursive_wrapper operator T() const and calling std visit in the interpreter. without a const T() const conversion, we cant call std visit with a const ref passing in the function. this i think is because we are extracting a ref to the actual expression object and it needs to be const to pass into a const ref function.



Possible Optimizations
- When parsing, we know that we have variable declarations and how many. so we can make a flat_map based on that with the right amount of reserved memory. this will make adding the definitions to a map at interpreting time quicker. We will start with map/unordered map first though and measure the change.

possible future changes to language to make it more c++ like the way I want to see c++ in the future.
- make var = init obsolete. make all init use the {} init syntax.

benefits of recursive wrapper vs using pointers.
- its a lot easier to serialize as the wrapper is now and index into a vector of expression nodes. granted its whole program vector right now but if we ever split it up into more granular mappings between vectors->statements, for example, it becomes easier to serialize individual components.
