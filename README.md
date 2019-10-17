# cpplox
A c++ implementation of the lox interpreter outlined at craftinginterpreters.com


1) create a build folder
2) run 'cmake ..' (by default, it build in debug mode but optionally you can run 'cmake -DCMAKE_BUILD_TYPE=Release' or if on windows, release is specified as a build option shown in step 3 below )
3) run 'cmake --build .' (or on windows ''cmake --build . --config Release')
4) On windows the executable will be '.\Debug\cpplox.exe' if you have built debug or '.\Release\cpplox.exe' for release
