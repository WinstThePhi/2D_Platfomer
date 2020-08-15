@echo off

REM variable setting
set warnings=/WX /W4 /wd4201 /wd4100 /wd4189

REM sum defines
set defines=/DHANDMADE_INTERNAL=1 /DHANDMADE_SLOW=1

REM compiler flags
set compilerFlags=/EHa /GR /Zi /FC /nologo /O2

REM libs
set libs=user32.lib gdi32.lib 

set platformBuildTarget=..\code\win32_handmade.cpp
set gameBuildTarget=..\code\handmade.cpp

if not exist build\ md build\

pushd build

cl %warnings% %defines% %compilerFlags% %gameBuildTarget% /Fmgame.map /link /DLL /EXPORT:GameUpdateAndRender /out:handmade.dll

cl %warnings% %defines% %compilerFlags% %platformBuildTarget% /link %libs%

popd
