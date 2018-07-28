#include "soundlib.h"
#include "soundlib_ctl.h"
#include "soundlib_sig.h"
#include "pa.h"

void disp(Msg m){ 
	for(unsigned int i = 0; i < m.num; i++){
		printf("%u:%u ", m.value[i]._n.note, m.value[i]._n.on);
	}printf("\r");
}

Mouse* m;
void paFunc(const float* in, float* out, unsigned long frames, void* data){    
	Osc* o = (Osc*)data;
    
    for(unsigned long i = 0; i < frames; i++){
    	*out++ = o->out(m->m.value[1]._f*1000);
    }
}


int main(){

	sl_init();

	m = new Mouse();

	// Key k;
	// Poly p;
	// k.connect(&p);
    PolyKey p;

	Osc* o = new Osc(200);

    Pa a(paFunc, o);
    a.start();

	while(1){
		call_ctl(); 
		 // printf("%f %f\r", m->m.value[0]._f, m->m.value[1]._f);
		disp(p.m);
		Sleep(10);
	}	
	a.terminate();	 

	return 0;
}

// poly >> set voice..m