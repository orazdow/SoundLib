
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
    

    Osc mod(434); 
    Osc car(300);
    Sum s;
    Mult m; //<<<<<(val)

    Sig n(2000);
    mod.connect(&m);
    n.connect(&m);
    Sig nn(200);
    m.connect(&s);
    nn.connect(&s);
    s.connect(&car);


    // Osc lfo(1);
    // Mult m;
    // Sig gain(20);
    // lfo.connect(&m);
    // gain.connect(&m);
    // Sum s;
    // Sig freq(200);
    // m.connect(&s);
    // freq.connect(&s); 
    // Osc car;
    // s.connect(&car);


    // for(int i = 0; i < 20; ++i){
    //         call_sig();
    //         printf("%f\n", *car.input);
    // }

    Pa a(paFunc, &car);
    a.start(Pa::waitForKey);


    return 0;
}

void printbus(Sig* n, uint inlet){ 
    // if(auto_summing) //<<should go into call()
    //     n->sumInputs();
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