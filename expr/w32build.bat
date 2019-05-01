@ECHO off

REM MSVC build bat file
REM run with vscmd: cmd /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"

set target=expr.exe
set builddir=winbuild

IF not [%1]==[] (GOTO args)

mkdir %builddir%

rem SET libdirs=/LIBPATH:D:\libraries\portaudio\build\msvc
rem SET libs=portaudio_x86.lib 

rem SET includes=/ID:\libraries\portaudio\include 

set debug=/Zi /DEBUG
rem set static=/MT

SET files=main.cpp expr.cpp lexer.cpp parser.cpp

SET opts=/Fe%target% /Fo%builddir%\ %static% %debug% /EHsc /std:c++17

cl %opts% %includes% %files% /link %libdirs% %libs%

GOTO end

:args
IF [%1]==[clean] (rmdir /s /q %builddir% & del %target%)
IF [%1]==[run] (%target%)

:end