#ifndef OSIO_H
#define OSIO_H

#define WIN32 1
#define OSX 0
#define LINUX 0

static long mouseX = 0;
static long mouseY = 0;

static inline bool getKeyState(unsigned int key);
static unsigned int getScreenWidth();
static unsigned int getScreenHeight();
static inline void getCursorPos();
static inline void setConsolePos(short x, short y);
static inline void clearScr();
void setCursorVis(bool visibility);
void initConsole();
struct ConsoleBuffer;
static inline void writeConsoleBuffer(ConsoleBuffer* b);
#define set_buff_char(_buffer, _index, _char)

void (*END_CB)() = NULL;

void setEndCb(void(*cb)(void)){
    END_CB = cb;
}

#if WIN32

	#include "windows.h"
	#pragma comment (lib, "user32.lib")


	static POINT point;
	static HANDLE CONSOLE, CONSOLE_IN;
	WORD charAttributes = 7;
	COORD origin = {0,0};
	BOOL CtrlHandler(DWORD fdwCtrlType);
	#undef set_buff_char
 	#define set_buff_char(_buffer, _index, _char) _buffer[_index].Char.AsciiChar = _char

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

	static inline void setConsolePos(short x, short y){
	    COORD pos = {x,y};
	    SetConsoleCursorPosition(CONSOLE, pos);
	}

	static inline void clearScr(){
	    system("CLS");
	}

	void initConsole(){
		CONSOLE = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_IN = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
		CONSOLE_SCREEN_BUFFER_INFO cInfo;
		GetConsoleScreenBufferInfo(CONSOLE, &cInfo);
		charAttributes = cInfo.wAttributes;
	    setCursorVis(false);
	    clearScr();
	}

	void setCursorVis(bool visibility){
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(CONSOLE, &cursorInfo);
		cursorInfo.bVisible = visibility; 
		SetConsoleCursorInfo(CONSOLE, &cursorInfo);
	}

	struct ConsoleBuffer{
		CHAR_INFO* buffer;
		COORD dimensions = {0,0};
		COORD origin = {0,0};
		SMALL_RECT coordinates = {0,0,0,0};
		WORD attributes = 7;
		DWORD len = 0;

		ConsoleBuffer(SHORT xlen, SHORT ylen, SHORT xpos = 0, SHORT ypos = 0){
			dimensions.X = xlen;
			dimensions.Y = ylen;
			origin.X = xpos;
			origin.Y = ypos;
			len = xlen*ylen;
			attributes = charAttributes;
			buffer = new CHAR_INFO[len];
			for(int i = 0; i < len; i++){
				buffer[i].Attributes = attributes;
			}
			coordinates.Left = xpos;
			coordinates.Top = ypos;
			coordinates.Bottom = xpos+(ylen-1);
			coordinates.Right = ypos+(xlen-1);
		}	
		~ConsoleBuffer(){
			delete[] buffer;
		}
		void free(){
			delete[] buffer;
		}
	};

	static inline void writeConsoleBuffer(ConsoleBuffer* b){
		    WriteConsoleOutput(
		  		CONSOLE,
		  		b->buffer,
		  		b->dimensions,
		  		b->origin,
		  		&b->coordinates
  		);
	}

	BOOL CtrlHandler(DWORD fdwCtrlType){ 
		FlushConsoleInputBuffer(CONSOLE_IN);
		setCursorVis(true);
		clearScr();
		if(END_CB){ END_CB(); }
		return FALSE;  
	}

#endif



#endif /* OSIO_H */