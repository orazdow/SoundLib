#include "osio.h"
#include "windows.h"

long mouseX = 0;
long mouseY = 0;

void (*END_CB)() = 0;

void setEndCb(void(*cb)(void)){
	END_CB = cb;
}


#if OS_HOST == OS_WIN

	#include "windows.h"
	#pragma comment (lib, "user32.lib")

	POINT point;
	HANDLE CONSOLE, CONSOLE_IN;
	WORD charAttributes = 7;
	COORD origin = {0,0};

 	BOOL CtrlHandler(DWORD fdwCtrlType);

 	void usleep(const unsigned long ms){
 		Sleep(ms);
 	}

	bool getKeyState(unsigned int key){
		return (bool)GetAsyncKeyState(key);
	}

	unsigned int getScreenWidth(){
		return GetSystemMetrics(SM_CXSCREEN);
	}

	unsigned int getScreenHeight(){
		return GetSystemMetrics(SM_CYSCREEN);
	}

	void getCursorPos(){
		GetCursorPos(&point);
		mouseX = point.x;
		mouseY = point.y;
	}

	void setConsolePos(short x, short y){
	    COORD pos = {x,y};
	    SetConsoleCursorPosition(CONSOLE, pos);
	}

	void clearScr(){
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

	inline void writeConsoleBuffer(ConsoleBuffer* b){
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
#else
	// wrong file included for host os (io_win.cpp)
#endif