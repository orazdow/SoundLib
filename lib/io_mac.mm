#include "osio.h"

long mouseX = 0;
long mouseY = 0;

void (*END_CB)() = 0;

void setEndCb(void(*cb)(void)){
	END_CB = cb;
}


#if OS_HOST == OS_MAC

	#import <Foundation/Foundation.h>
	#import <AppKit/AppKit.h>

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

#else
	// wrong file included for host os (io_mac.mm)
#endif