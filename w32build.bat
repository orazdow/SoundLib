@ECHO off

REM MSVC build bat file
REM run with vscmd: cmd /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"

set target=slib.exe
set builddir=winbuild

IF not [%1]==[] (GOTO args)

mkdir %builddir%

SET libdirs=/LIBPATH:D:\libraries\portaudio\build\msvc
SET libs=portaudio_x86.lib 
rem SET libs=portaudio_static.lib 
SET includes=/ID:\libraries\portaudio\include 

set debug=/Zi /DEBUG
rem set static=/MT

SET files=main.cpp lib\pa.cpp
rem SET files=main.cpp 

SET opts=/Fe%target% /Fo%builddir%\ %static% %debug%

cl %opts% %includes% %files% /link %libdirs% %libs%

GOTO end

:args
IF [%1]==[clean] (rmdir /s /q %builddir% & del %target%)
IF [%1]==[run] (%target%)

:end