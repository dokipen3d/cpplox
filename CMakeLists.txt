cmake_minimum_required (VERSION 3.9)
project (cpplox)


option(CMAKE_BUILD_TYPE "Release or Debug" Release)


add_executable(cpplox Scanner.cpp main.cpp) 
