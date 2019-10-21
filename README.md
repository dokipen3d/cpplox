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
