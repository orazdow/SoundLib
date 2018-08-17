#ifndef OSIO_H
#define OSIO_H

#define WIN32 1
#define OSX 0
#define LINUX 0

static long mouseX = 0, mouseY = 0;

static inline bool getKeyState(unsigned int key);
static inline unsigned int getScreenWidth();
static inline unsigned int getScreenHeight();
static inline void getCursorPos();
static inline void setCursorPos(short x, short y);
static inline void clearScr();
void setCursorVis(bool visibility);
void initConsole();

#if WIN32

	#include "windows.h"

	static POINT point;
	static HANDLE CONSOLE;
	WORD defCharAttributes;
	COORD origin = {0,0};

	static inline bool getKeyState(unsigned int key){
		return (bool)GetAsyncKeyState(key);
	}

	static inline unsigned int getScreenWidth(){
		return GetSystemMetrics(SM_CXSCREEN);
	}

	static inline unsigned int getScreenHeight(){
		return GetSystemMetrics(SM_CYSCREEN);
	}

	static inline void getCursorPos(){
		GetCursorPos(&point);
		mouseX = point.x;
		mouseY = point.y;
	}

	static inline void setCursorPos(short x, short y){
	    COORD pos = {x,y};
	    SetConsoleCursorPosition(CONSOLE, pos);
	}

	static inline void clearScr(){
	    system("CLS");
	}

	void initConsole(){
		CONSOLE = GetStdHandle(STD_OUTPUT_HANDLE);
	    setCursorVis(false);
	    clearScr();
	}

	void setCursorVis(bool visibility){
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(CONSOLE, &cursorInfo);
		cursorInfo.bVisible = visibility; 
		SetConsoleCursorInfo(CONSOLE, &cursorInfo);
	}

#endif



#endif /* OSIO_H */