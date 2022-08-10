cmake_minimum_required (VERSION 3.9)
project (cpplox)


if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)


add_executable(cpplox main.cpp TokenTypes.cpp Resolver.cpp ExpressionPrinterVisitor.cpp ExpressionInterpreter.cpp ExpressionParser.cpp Error.cpp Scanner.cpp ) 
set_property(TARGET cpplox PROPERTY CXX_STANDARD 17)

if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET cpplox PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

#target_compile_options(cpplox PRIVATE -Werror -Wall -Wextra -Wno-error=switch -w)

