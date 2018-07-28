#include "soundlib.h"
#include "soundlib_ctl.h"
#include "soundlib_sig.h"
#include "pa.h"

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

Mouse* m;
void paFunc(const float* in, float* out, unsigned long frames, void* data){    
	Osc* o = (Osc*)data;
    
    for(unsigned long i = 0; i < frames; i++){
    	*out++ = o->out(m->y);
    }
}


int main(){

	sl_init();

	m = new Mouse();

	Key k;
	Poly p;
	Osc* o = new Osc(200);

    Pa a(paFunc, o);
    a.start();


	k.connect(&p);

	while(1){
		call_ctl(); 
		 // printf("%f %f\r", m->x, m->y);
		 disp(p.m);
	 //    disp_p(p);
		Sleep(10);
	}	
	a.terminate();	 

	return 0;
}

// poly >> set voice..m