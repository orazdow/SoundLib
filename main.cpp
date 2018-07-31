#include "soundlib.h"
#include "soundlib_ctl.h"
#include "soundlib_sig.h"
#include "pa.h"

void disp(Msg m){ 
	for(unsigned int i = 0; i < m.num; i++){
		 printf("%u:%u ", m.value[i]._n.note, m.value[i]._n.on);
		// printf("%u:%u ", m.keys[i].note, m.keys[i].ready());
	}printf("\r");
}

//Mouse* m;
void paFunc(const float* in, float* out, unsigned long frames, void* data){    
	
	Synth* s = (Synth*)data;
    
    for(unsigned long i = 0; i < frames; i++){
    	// *out++ = o->out(m->m.value[1]._f*1000);
    	 *out++ =  s->out()*0.7;
    }
}


int main(){

	sl_init();

	// m = new Mouse();

    PolyKey p(8);

	// Osc* o = new Osc(200);
	// TestVoice* v = new TestVoice();

	// p.connect(v);
	//p.setEnv(v->getEnv(), 0);
	// Env* e = v->getEnv();
	// p.setEnv(&e, 1);
	// printf("%s\n", v->getEnv()->ha);
	Synth* s = new Synth;
	p.connect(s);
	p.setEnvs(s->getEnvs(), 8);
    Pa a(paFunc, s);
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

