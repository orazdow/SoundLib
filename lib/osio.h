#ifndef OSIO_H
#define OSIO_H

#define OS_WIN 1
#define OS_MAC 2
#define OS_GNU 3

#ifndef OS_HOST
	#define OS_HOST OS_WIN
#endif

extern long mouseX;
extern long mouseY;
extern void (*END_CB)(void);

struct ConsoleBuffer;
#define set_buff_char(_buffer, _index, _char)

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


#if OS_HOST==OS_WIN

	#include "windows.h"
	#pragma comment (lib, "user32.lib")

	extern POINT point;
	extern HANDLE CONSOLE, CONSOLE_IN;
	extern WORD charAttributes;
	extern COORD origin;

	#undef set_buff_char
 	#define set_buff_char(_buffer, _index, _char) _buffer[_index].Char.AsciiChar = _char
 	BOOL CtrlHandler(DWORD fdwCtrlType);
 	void usleep(const unsigned long ms);
 	
#elif OS_HOST==OS_MAC
	#import <Foundation/Foundation.h>
	#import <AppKit/AppKit.h>

#elif OS_HOST==OS_GNU
	#include "uistd.h"

#endif

#endif /* OSIO_H */