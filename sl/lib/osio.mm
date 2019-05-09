
#include "osio.h"

long mouseX = 0;
long mouseY = 0;

void (*END_CB)() = 0;

void setEndCb(void(*cb)(void)){
	END_CB = cb;
}


 #if OS_OSX

 	void sleepMs(unsigned long ms){
 		sleep(ms);
 	}

	unsigned int getScreenWidth(){
		return [NSScreen mainScreen].frame.size.width;
	}

	unsigned int getScreenHeight(){
		return [NSScreen mainScreen].frame.size.height;
	}

	void getCursorPos(){
		mouseX = [NSEvent mouseLocation].x;
		mouseY = [NSEvent mouseLocation].y;
	}

 #endif

#if OS_WIN32

 	void sleepMs(unsigned long ms){
 		Sleep(ms);
 	}

	unsigned int getScreenWidth(){
		return GetSystemMetrics(SM_CXSCREEN);
	}

	inline unsigned int getScreenHeight(){
		return GetSystemMetrics(SM_CYSCREEN);
	}

	inline void getCursorPos(){
		GetCursorPos(&point);
		mouseX = point.x;
		mouseY = point.y;
	}

	inline void setConsolePos(short x, short y){
	    COORD pos = {x,y};
	    SetConsoleCursorPosition(CONSOLE, pos);
	}

	inline void clearScr(){
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


#endif