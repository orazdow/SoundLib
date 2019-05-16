#include "osio.h"

long mouseX = 0;
long mouseY = 0;

void (*END_CB)() = 0;

void setEndCb(void(*cb)(void)){
	END_CB = cb;
}

#if OS_HOST == OS_GNU
	#include "uistd.h"

#else
	// wrong file included for host os (io_gnu.cpp)
#endif
