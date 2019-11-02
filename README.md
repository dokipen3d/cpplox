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

for running clang tidy on windows, follow these instructions....
https://bitsmaker.gitlab.io/post/clang-tidy-from-vs2015/

basically means installing ninja (putting the exe in the path), calling the varsall.bat then generating the ninja files with the compile commands json. then you can use clang tidy with the json file and -p argument.

Some things that I learned along the way....

static function means that only the current TU can use it. its not visible by other TUs at link time. (although can still be called through function pointers).

calling std get with a const ref to a variant will return a const refT back. the return constness of the return type depends on the type of the variant passed in. kind sof makes sense. you dont want to return a non const ref to a const variant from std get. the wrapper function needs to return a const ref too.

making a function const only makes sense for a template function that is part of a class. it means that it wont modify the class itself. a free template function doesnt need const because its not a member of anythin it can moidy. this is why we got  Error C2270: Modifiers not allowed on nonmember functions

- i first tried inheriting from variant to add conversion functions and comparison operators. this worked for complete types but the variant with the recursive wrapper didnt work due to some issue with the gcc implementation using std::variant size and that not being able to use incomplete types. i learned that I could have free comparison operators! no need to inherit, just define them. it does make the code a bit unweildy as they leak out from the definition of the class designed to keep things together.

- To make a variant recursive, you have to use allocation to be able to use incomplete types (that has the variant itself as a member). This way the storage of the members is on the heap and not inline in the types of the variant. This gets round the issue of types that contain the same types (which will contain the same type) and so on. You can't obviously have infinite sized types.

- the storage for the recursive types is done using a vector in the recursuve wrapper. the naive implementation uses a single std vector for each wrapped type. What we can do instead is make the vector static, define it (so that its in a TU and inline so that all TUs that include the hpp have the same one defined for all THATS WHAT THE INLINE KEYWORD DOES). This makes a static vector for each type (T). This is whole program though so all expressions accross the whole program will use the same one. 
TODOs will be to 1) somehow break that up into more vectors so that its one vec per statement or per TU or something. 2) Clear the vectors when the expressions are no longer needed. 
Also we have dynamically allocated Statements which themselves refer to Expr, so they could potentially provide the storage for the expressions they contain. But there is no easy way for the Expr to know which vector they will be pushing back to. Unless instead of a static vector, there is a static function that tells the 'current' expression where to store its data.  maybe this is overkill.

- const is hard thing to get right, especially when getting conversion functions and  constructors/assingment operators right in wrapper calling visit on a variant. see recursive_wrapper operator T() const and calling std visit in the interpreter. without a const T() const conversion, we cant call std visit with a const ref passing in the function. this i think is because we are extracting a ref to the actual expression object and it needs to be const to pass into a const ref function.

- compiler messages are not helpful
 error C2678: binary '=': no operator found which takes a left-hand operand of type 'const std::vector<cpplox::Statement,std::allocator<_T
y>>' (or there is no acceptable conversion) [C:\Users\dokipen\Documents\Projects\cpplox\build\cpplox.vcxproj]

i was trying to assign to a const vector. just say that!

delliott@agusta211b: /usr/home/delliott/Documents/Projects/cpplox/build                                                                                                                        {13} > make
Scanning dependencies of target cpplox
[ 14%] Building CXX object CMakeFiles/cpplox.dir/ExpressionInterpreter.cpp.o
In file included from /usr/home/delliott/Documents/Projects/cpplox/TokenTypes.h:6:0,
                 from /usr/home/delliott/Documents/Projects/cpplox/Expr.hpp:4,
                 from /usr/home/delliott/Documents/Projects/cpplox/ExpressionInterpreter.h:2,
                 from /usr/home/delliott/Documents/Projects/cpplox/ExpressionInterpreter.cpp:1:
/usr/include/c++/7/variant: In instantiation of ‘static constexpr auto std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...)>, std::tuple<_Args2 ...>, std::integer_sequence<long unsigned int, __indices ...> >::_S_apply() [with _Result_type = void; _Visitor = cpplox::Interpreter&; _Variants = {const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&}; long unsigned int ...__indices = {3}]’:
/usr/include/c++/7/variant:671:61:   required from ‘static constexpr void std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...), __dimensions ...>, std::tuple<_Variants ...>, std::integer_sequence<long unsigned int, __indices ...> >::_S_apply_single_alt(_Tp&) [with long unsigned int __index = 3; _Tp = std::__detail::__variant::_Multi_array<void (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&)>; _Result_type = void; _Visitor = cpplox::Interpreter&; long unsigned int ...__dimensions = {4}; _Variants = {const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&}; long unsigned int ...__indices = {}]’
/usr/include/c++/7/variant:659:39:   required from ‘static constexpr void std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...), __dimensions ...>, std::tuple<_Variants ...>, std::integer_sequence<long unsigned int, __indices ...> >::_S_apply_all_alts(std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...), __dimensions ...>, std::tuple<_Variants ...>, std::integer_sequence<long unsigned int, __indices ...> >::_Array_type&, std::index_sequence<__indices ...>) [with long unsigned int ...__var_indices = {0, 1, 2, 3}; _Result_type = void; _Visitor = cpplox::Interpreter&; long unsigned int ...__dimensions = {4}; _Variants = {const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&}; long unsigned int ...__indices = {}; std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...), __dimensions ...>, std::tuple<_Variants ...>, std::integer_sequence<long unsigned int, __indices ...> >::_Array_type = std::__detail::__variant::_Multi_array<void (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&), 4>; std::index_sequence<__indices ...> = std::integer_sequence<long unsigned int, 0, 1, 2, 3>]’
/usr/include/c++/7/variant:649:19:   required from ‘static constexpr std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...), __dimensions ...>, std::tuple<_Variants ...>, std::integer_sequence<long unsigned int, __indices ...> >::_Array_type std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...), __dimensions ...>, std::tuple<_Variants ...>, std::integer_sequence<long unsigned int, __indices ...> >::_S_apply() [with _Result_type = void; _Visitor = cpplox::Interpreter&; long unsigned int ...__dimensions = {4}; _Variants = {const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&}; long unsigned int ...__indices = {}; std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...), __dimensions ...>, std::tuple<_Variants ...>, std::integer_sequence<long unsigned int, __indices ...> >::_Array_type = std::__detail::__variant::_Multi_array<void (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&), 4>]’
/usr/include/c++/7/variant:708:38:   required from ‘static constexpr std::__detail::__variant::__gen_vtable<_Result_type, _Visitor, _Variants>::_Array_type std::__detail::__variant::__gen_vtable<_Result_type, _Visitor, _Variants>::_S_apply() [with _Result_type = void; _Visitor = cpplox::Interpreter&; _Variants = {const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&}; std::__detail::__variant::__gen_vtable<_Result_type, _Visitor, _Variants>::_Array_type = std::__detail::__variant::_Multi_array<void (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&), 4>; typename std::remove_reference<_Variants>::type = <type error>]’
/usr/include/c++/7/variant:711:49:   required from ‘constexpr const std::__detail::__variant::_Multi_array<void (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&), 4> std::__detail::__variant::__gen_vtable<void, cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&>::_S_vtable’
/usr/include/c++/7/variant:711:29:   required from ‘struct std::__detail::__variant::__gen_vtable<void, cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&>’
/usr/include/c++/7/variant:1255:23:   required from ‘constexpr decltype(auto) std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = cpplox::Interpreter&; _Variants = {const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&}]’
/usr/home/delliott/Documents/Projects/cpplox/ExpressionInterpreter.cpp:30:41:   required from here
/usr/include/c++/7/variant:693:43: error: invalid conversion from ‘std::__success_type<cpplox::Object>::type (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&) {aka cpplox::Object (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&)}’ to ‘void (*)(cpplox::Interpreter&, const std::variant<cpplox::ExpressionStatement, cpplox::PrintStatement, cpplox::VariableStatement, std::monostate>&)’ [-fpermissive]
       { return _Array_type{&__visit_invoke}; }
                                           ^
CMakeFiles/cpplox.dir/build.make:88: recipe for target 'CMakeFiles/cpplox.dir/ExpressionInterpreter.cpp.o' failed
make[2]: *** [CMakeFiles/cpplox.dir/ExpressionInterpreter.cpp.o] Error 1
CMakeFiles/Makefile2:75: recipe for target 'CMakeFiles/cpplox.dir/all' failed
make[1]: *** [CMakeFiles/cpplox.dir/all] Error 2
Makefile:83: recipe for target 'all' failed
make: *** [all] Error 2

hadnt made a visitor function for monostate


Possible Optimizations
- When parsing, we know that we have variable declarations and how many. so we can make a flat_map based on that with the right amount of reserved memory. this will make adding the definitions to a map at interpreting time quicker. We will start with map/unordered map first though and measure the change.

possible future changes to language to make it more c++ like the way I want to see c++ in the future.
- make var = init obsolete. make all init use the {} init syntax.

benefits of recursive wrapper vs using pointers.
- its a lot easier to serialize as the wrapper is now and index into a vector of expression nodes. granted its whole program vector right now but if we ever split it up into more granular mappings between vectors->statements, for example, it becomes easier to serialize individual components.


drawbacks? 
variant has to have all types known up front so no extending by user (unless you expose a template to ingest new types). actually I quite like the explicit grouping of types, especially for a grammer like this as it removes any chance for missing something. 

style things I liked....
- trailing return types so headers have all functions lined up. you can sort by bool, void and auto then sort the autos by the right hand side.
- wish there was a clang format for aligning trainign return types on the right 

cant have a combined visitor that can visit multiple variants that share monostate as you cant override the function for both   