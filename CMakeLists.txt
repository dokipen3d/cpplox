cmake_minimum_required (VERSION 3.9)
project (cpplox)


if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

add_executable(cpplox main.cpp ExpressionInterpreter.cpp ExpressionParser.cpp Error.cpp Scanner.cpp ) 
set_property(TARGET cpplox PROPERTY CXX_STANDARD 17)
