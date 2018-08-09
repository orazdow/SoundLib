#include "soundlib.h"
#include "soundlib_ctl.h"
#include "soundlib_sig.h"
#include "pa.h"

void disp(Msg m){ 
	for(unsigned int i = 0; i < m.num; i++){
		 printf("%u:%u ", m.value[i]._n.note, m.value[i]._n.on);
	}printf("\r");
}

void paFunc(const float* in, float* out, unsigned long frames, void* data){    
	
	Synth* s = (Synth*)data;
    
    for(unsigned long i = 0; i < frames; i++){ 
    	 call_sig();
    	*out++ =  s->out()*1.5;
    }
}


int main(){

	sl_init();

    PolyKey p(8);
	Synth s(8);
	p.connect(&s);

	VuMeter v;

	s.connect(&v);
	

    Pa a(paFunc, &s);
    a.start();

	while(1){
		call_ctl(); 
		
	    // disp(p.m);
		Sleep(10);
	}	
	a.terminate();	 

	return 0;
}

