@echo off
if [%1]==[] goto usage

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"

pushd %1
cmake -DCMAKE_C_COMPILER="clang-cl.exe" -DCMAKE_CXX_COMPILER="clang-cl.exe" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G "Ninja" ..
popd

@echo Done.
goto :eof

:usage
@echo Usage: %0 ^<full path to ninja build directory^>
exit /B 1

:eof