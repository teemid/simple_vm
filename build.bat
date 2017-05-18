@echo off

SET SOURCE_DIR=%~dp0

pushd build

cl %SOURCE_DIR%\main.cpp /EHsc /Zi /link /OUT:simple_vm.exe

popd
