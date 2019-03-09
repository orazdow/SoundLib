
#include "soundlib.h"
#include "soundlib_sig.h"
#include "lib/pa.h"
#include "stdio.h"

void printbus(Sig* n, uint inlet);
void printchilds(Sig* n);
void dfs(Sig* n, int lev);

void paFunc(const float* in, float* out, unsigned long frames, void* data){    

     Osc* o = (Osc*)data;
     //Sum* o = (Sum*)data;
    
    for(unsigned long i = 0; i < frames; i++){ 
        call_sig();
        *out++ =  o->output*0.33;
    }
}

int main(){

    sl_init();
    

/*  // fm test with Bus
    Osc mod(434); 
    Osc car(300);
    Bus s;
    Bus m(sl::mult); 

    Sig n(2000);
    mod.connect(&m);
    n.connect(&m);
    Sig nn(200);
    m.connect(&s);
    nn.connect(&s);
    s.connect(&car);
*/
 
  // fm test Sum, Mult
    Osc mod(434); 
    Osc car;
    Mult m(2000); 
    mod.connect(&m);
    int fundamental = 200;
    Sum s(fundamental);
    m.connect(&s);
    s.connect(&car);
    // adding lfo
    Osc lfo(1);
    Sum s2(1); // need expr
    lfo.connect(&s2); // need expr
    Mult amp(2000);
    s2.connect(&amp);
    // replace default at right inlet
    amp.connect(&m, 1); 

/* // Sum as Bus, mult using inlet
   // put op functionality sig...
    Osc mod(434); 
    Osc car(300);
    Sum s;
    Mult m;

    Sig n(2000);
    mod.connect(&m);
    n.connect(&m, 1);
    Sig nn(200);
    m.connect(&s);
    nn.connect(&s);
    s.connect(&car);
*/

/*  // Sum val test
    Sum s(50);
    Sig a(12);

    a.connect(&s);
    call_sig();
    printf("%f\n", s.output);
    Sig b(60);
    b.connect(&s, 1);
    call_sig();
    printf("%f\n", s.output);
*/

/*  // Mult val test
    Mult m(8);
    Sig a(3);

    a.connect(&m);
    call_sig();
    printf("%f\n", m.output);
    Sig b(12);
    b.connect(&m, 1);
    call_sig();
    printf("%f\n", m.output);
*/
    // for(int i = 0; i < 20; ++i){
    //         call_sig();
    //         printf("%f\n", *car.input);
    // }

    // for(int i = 0; i < 20; ++i){
    //         call_sig();
    //         printf("%f\n", *car.input);
    // }

    Pa a(paFunc, &car);
    a.start(Pa::waitForKey);


    return 0;
}

void printbus(Sig* n, uint inlet){ 
    printf("%f|", *n->inputs[inlet]);
    uint end = n->input_bus->float_map.addptr[inlet];
    uint n_summing = n->input_bus->n_summing;
    for(int i = 0; i < end; i++){
        printf("%f, ", *(n->input_bus->inputs[i+inlet*n_summing]));
    }
    printf("\n");
}


void printchilds(Sig* n){
    for(int i = 0; i < n->child_map->addptr; i++){
        printf("%u, ", n->child_map->nodes[i]->id);
    } printf("\n");

    // for( auto p : n->childs){
    //      printf("%u, ", p.second->id);
    // }   printf("\n");
}