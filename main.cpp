
#include "lib/osio.h"
#include "lib/pa.h"
#include "soundlib.h"
#include "soundlib_sig.h"
#include "soundlib_ctl.h"
#include "expr.h"


void paFunc(const float* in, float* out, unsigned long frames, void* data){    

     Sig* o = (Sig*)data;
    
    for(unsigned long i = 0; i < frames; i++){ 
        call_sig();
        *out++ =  o->output*0.7;
    }
}


int main(){

    sl_init();
    
 
    Sig out;


    Osc mod(434);
    Osc car;

    FnGen lfo(sl::sin, 1);
    // Sig freq(100);
    FnGen lfo2(sl::saw, 0.18);

    Expr expr("f0* ( (1+f1) *f3*2000 ) + (1+f2)*120");

    mod.connect(&expr);
    lfo.connect(&expr, 1);
    lfo2.connect(&expr, 2);
    expr.connect(&car);

    car.connect(&out);

    Mouse m;

    Mult mm(20);

    sig_connect(&m.x, &mm);

    mm.connect(&lfo);

    sig_connect(&m.y, &expr, 3);

    lfo2.disconnect(&expr, 2);

    // voice patch test
    // PolyKey p(1);
    // TestVoice v;
    // p.connect(&v);


    Pa a(paFunc, &out);
    a.start();

    while(1){
        call_ctl(); 
        // usleep(20);
    }

    a.terminate();

    return 0;
}