#include "soundlib.h"
#include "soundlib_ctl.h"

void disp(Msg m){ 
	for(unsigned int i = 0; i < m.num; i++){
		printf("%u:%u ",(m.value[i]>>16), (m.value[i]&0xffff));
	}printf("\r");
}

void disp_p(Poly p){
    for(int i =0; i < p.polylimit; i++){
       printf("%u:%u ",p.keys[i].note, !p.keys[i].ready());
    }  printf("\r"); 
}

int main(){

	sl_init();

	Key k;
	Poly p;

	k.connect(&p);

	while(1){
		call_ctl(); 
		 disp(p.m);
	 //    disp_p(p);
		Sleep(30);
	}		 

	return 0;
}

// poly >> set voice..m