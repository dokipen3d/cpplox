cmake_minimum_required (VERSION 3.9)
project (cpplox)




add_executable(cpplox Error.cpp Expression.cpp Scanner.cpp main.cpp) 
set_property(TARGET cpplox PROPERTY CXX_STANDARD 17)
