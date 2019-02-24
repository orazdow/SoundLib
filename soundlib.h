#ifndef SOUNDLIB_H
#define SOUNDLIB_H

#include "soundlib_glob.h"
#include "map.h"

// return ptr..
void sig_connect(Sig* a, Sig* b, uint inlet = 0);
void sig_disconnect(Sig* a, Sig* b, uint inlet = 0);

class Sig{ 
public:

    uint id;
    //Sig* parent = NULL;
    uint parents = 0;
    bool master = 0;
    uint inlets = 8; // set in derived, def 1

    float* input;
    float output = 0;
    float** inputs; 
    Bus* input_bus;
    Node_map<Sig>* child_map;

    void connect(Sig* child, uint inlet = 0){ sig_connect(this, child, inlet);}
    void disconnect(Sig* child, uint inlet = 0){ sig_disconnect(this, child, inlet);}

     Sig() : Sig(0.0f){ } //...
     Sig(int n) : Sig((float)n){ }
     Sig(double n) : Sig((float)n){ }

  //  Sig(uint inlets, uint summing){} //Sig(uint inlets, uint summing = 1){}

    Sig(bool _master = 0){
        if(_master)
        if(!GLOB_SIG_SET){ 
            master = 1;
            GLOB_SIG_SET = 1;
        }
    
        init(0.0f, num_inlets, num_summing, map_limit);

        if(!_master && !SWITCH_CHAIN_INDEPENDENT){
           glob_sig->connect(this); 
        }
    }

    Sig(float val){ 
    	init(val, num_inlets, num_summing, map_limit);

        if(!SWITCH_CHAIN_INDEPENDENT){
           glob_sig->connect(this); 
        }
    }

    void init(float f, uint n_inlets, uint n_summing, uint n_map){
    	this->id = ++g_id; 
        output = f;
        inputs = new float*[n_inlets];
        input_bus  = new Bus(n_inlets, n_summing);
        child_map = new Node_map<Sig>(n_map);

        for(int i = 0; i < n_inlets; i++)
            inputs[i] = &input_bus->outputs[i]; 

        input = inputs[0];   	
    }

    void sumInputs(){
        for(int i = 0; i < inlets; i++)
            input_bus->sum(i);
    }

    void setInlets(uint n){
        inlets = n;
    }

  // void init_summing(){}

    ~Sig(){ delete[] inputs; delete child_map; delete input_bus; }

};

void sig_connect(Sig* a, Sig* b, uint inlet){
    // add inlet warning

    if(!a->master){ glob_sig->disconnect(b); } 

    if(!b->parent && inlet == 0){ // < condition not needed for sorted queue (connect everything) <remove after sortqueue implemented>
        a->child_map->add(b, b->id);
        b->parents++;
    }

    b->input_bus->add(&a->output, a->id, inlet);

    if(!auto_summing)
        b->inputs[inlet] = b->input_bus->inputs[inlet*num_summing];
}

void sig_disconnect(Sig* a, Sig* b, uint inlet){

    a->child_map->remove(b->id);
    dec_limit(b->parents, 0);

    if(b->parents == 0 && !a->master){ Glob_Sig->connect(b); }    

    b->input_bus->remove(a->id, inlet); 

    if(!auto_summing)
        b->inputs[inlet] = b->input_bus->inputs[inlet*num_summing];
}

/***** Sig root class *****/
class Glob_Sig : public Sig{
public:
    Glob_Sig() : Sig(true){}
   // void dsp(){ callChildren(); }
};

/********  init  *************/
void sl_init(){
	printf("initing..\n");
   // glob_ctl = new Glob_Ctl();
    glob_sig = new Glob_Sig();
    init_globals();
}

#endif //SOUNDLIB_H