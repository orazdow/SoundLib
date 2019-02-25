
#include "soundlib.h"
#include "soundlib_sig.h"
#include "lib/pa.h"
#include "stdio.h"

// void printbus(Sig* n, uint inlet);
// void printchilds(Sig* n);
// void dfs(Sig* n, int lev);

void paFunc(const float* in, float* out, unsigned long frames, void* data){    

    // Osc* o = (Osc*)data;
     Sum* o = (Sum*)data;
    
    for(unsigned long i = 0; i < frames; i++){ 
        call_sig();
        *out++ =  o->output*0.33;
    }
}

int main(){

    sl_init();

    // Osc o(200);

    // Pa a(paFunc, &o);
    // a.start(Pa::waitForKey);

    Osc mod(434); 
    Osc car(200);
    // Sum s;
    // Mult m; //<<<<<(val)

    // Sig n(2000);
    // mod.connect(&m);
    // n.connect(&m);
    // Sig nn(200);
    // m.connect(&s);
    // nn.connect(&s);
    // s.connect(&car);
    // Pa a(paFunc, &car);

    Sum s;

    mod.connect(&s);
    car.connect(&s);
    Pa a(paFunc, &s);
    a.start(Pa::waitForKey);

    return 0;

}