@echo off

if not exist build\ md build\

pushd ..\build

cl /Zi /FC ..\code\win32_handmade.cpp user32.lib gdi32.lib kernel32.lib

popd
