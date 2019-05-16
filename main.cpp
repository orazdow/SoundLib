
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

    // Osc mod(434); 
    // Osc car;
    // Mult m(2000); 
    // mod.connect(&m);
    // int fundamental = 100;
    // Sum s(fundamental);
    // m.connect(&s);
    // s.connect(&car);
    // // adding lfo
    // FnGen lfo(sl::saw, 1);
    // Sum s2(1); // need expr
    // lfo.connect(&s2); // need expr
    // Mult amp(2000);
    // s2.connect(&amp);
    // // replace default at right inlet
    // amp.connect(&m, 1); 

    // Lp filt(300);
    // // add filter and cutoff lfo
    // FnGen filtlfo(sl::sin, 6);
    // Sum s3(1.5); // need expressions....
    // filtlfo.connect(&s3); //
    // Mult mm(1000); //
    // s3.connect(&mm); // 
    // mm.connect(&filt, 1);

    // car.connect(&filt);

    // filt.connect(&out);

    Osc mod(434);
    Osc car;

    FnGen lfo(sl::sin, 1);
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
    // v.connect(&out);


    Pa a(paFunc, &out);
    a.start();

    while(1){
        call_ctl(); 
        // usleep(20);
    }

    a.terminate();

    return 0;
}