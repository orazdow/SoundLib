@ECHO off

REM MSVC build bat file
REM run with vscmd: cmd /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"

set target=slib.exe
set builddir=winbuild

IF not [%1]==[] (GOTO args)

mkdir %builddir%

rem SET libdirs=/LIBPATH:portaudio_win\build
rem SET libs=portaudio_x86.lib 

rem SET includes=/Iportaudio_win\include /Ilib

set debug=/Zi /DEBUG

SET files=main.cpp 

SET opts=/Fe%target% /Fo%builddir%\ %debug%
rem SET opts=/Fe%builddir%/%target% /Fo%builddir%/

cl %opts% %includes% %files% /link %libdirs% %libs%

GOTO end

:args
IF [%1]==[clean] (del %builddir%\* del %target%)
IF [%1]==[run] (%target%)

:end
