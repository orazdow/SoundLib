#ifndef OSIO_H
#define OSIO_H

#define OS_WIN32 0
#define OS_OSX 1
#define OS_LINUX 0

extern long mouseX;
extern long mouseY;
extern void (*END_CB)(void);
struct ConsoleBuffer;

#define set_buff_char(_buffer, _index, _char)

void sleepMs(unsigned long ms);
bool getKeyState(unsigned int key);
unsigned int getScreenWidth();
unsigned int getScreenHeight();
void getCursorPos();
void setConsolePos(short x, short y);
void clearScr();
void setCursorVis(bool visibility);
void initConsole();
void writeConsoleBuffer(ConsoleBuffer* b);
void setEndCb(void(*cb)(void));

#if OS_OSX

	#import <Foundation/Foundation.h>
	#import <AppKit/AppKit.h>
	#include <unistd.h>

#endif

#if OS_WIN32

	#include "windows.h"
	#pragma comment (lib, "user32.lib")

	extern POINT point;
	extern HANDLE CONSOLE, CONSOLE_IN;
	WORD charAttributes = 7;
	COORD origin = {0,0};
	BOOL CtrlHandler(DWORD fdwCtrlType);

	#undef set_buff_char
 	#define set_buff_char(_buffer, _index, _char) _buffer[_index].Char.AsciiChar = _char
 	
#endif

#endif /* OSIO_H */