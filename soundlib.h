#ifndef SOUNDLIB_H
#define SOUNDLIB_H

#include "soundlib_glob.h"
#include "map.h"

void sig_connect(Sig* a, Sig* b, uint inlet = 0);
void sig_disconnect(Sig* a, Sig* b, uint inlet = 0);

class Sig{ 
public:

    uint id;
    uint parents = 0;
    bool master = 0;
    uint inlets = 8; // set in derived, def 1

    float* input;
    float** inputs;
    float val = 0;
    float output = 0;
    Bus* input_bus;
    //std::map <int, Sig*> childs;
    Node_map<Sig>* child_map;


    Sig(double val){ 
        if(GLOB_NODE_INIT){
            master = 1;
            GLOB_NODE_INIT = 0;
        }
        this->id = g_id++;  
        init((float)val, num_inlets, num_summing, map_limit);

        if(!master && !SWITCH_CHAIN_INDEPENDENT){
           glob_sig->connect(this); 
        }
    }

    Sig() : Sig(0){ } 
  //  Sig(uint inlets, uint summing){} //Sig(uint inlets, uint summing = 1){}

    ~Sig(){ delete[] inputs; delete input_bus; }

    void init(float f, uint n_inlets, uint n_summing, uint n_map){
        inputs = new float*[n_inlets];
        input_bus  = new Bus(n_inlets, n_summing);
        child_map = new Node_map<Sig>(n_map); // not needed if using std:map
        for(int i = 0; i < n_inlets; i++)
            inputs[i] = &input_bus->outputs[i]; 

        input = inputs[0];   
        val = f;
        output = val;   

        input_bus->update(&val, 0);
        // input_bus->inputs[0] = &val;
        // input_bus->outputs[0] = *(input_bus->inputs[0]);

    }

    virtual void dsp(){ output = *input; }
    virtual float out(){ return output = *input; }
    virtual float out(double in){ return output = (float)in; }

    // return ptr...
    void connect(Sig* child, uint inlet = 0){ sig_connect(this, child, inlet);}
    void disconnect(Sig* child, uint inlet = 0){ sig_disconnect(this, child, inlet);}

    void sumInputs(){
        for(int i = 0; i < inlets; i++)
            input_bus->sum(i);
    }

    void setInlets(uint n){
        inlets = n;
    }

  // void init_summing(){}

    void call(){ 
        /* if auto_summin sum inputs */
        dsp();
        callChildren();     
    }

    void callChildren(){ 
        // for(auto p : childs){
        //     p.second->call();
        // }
        for(int i = 0; i < child_map->addptr; i++){
            child_map->nodes[i]->call();
        }
    } 

};

void sig_connect(Sig* a, Sig* b, uint inlet){
    // add inlet warning

    if(!a->master){ glob_sig->disconnect(b); } 

    if(!b->parents && inlet == 0){ // < condition not needed for sorted queue (connect everything) <remove after sortqueue implemented>
       // a->childs[b->id] = b;
        a->child_map->add(b, b->id);
        b->parents++;
    }

    b->input_bus->add(&a->output, a->id, inlet);

}

void sig_disconnect(Sig* a, Sig* b, uint inlet){

    a->child_map->remove(b->id);
    //a->childs.erase(b->id); 
    dec_limit(b->parents, 0);

    if(!b->parents && !a->master){ glob_sig->connect(b); }    

    b->input_bus->remove(a->id, inlet); 

}


/********  init  *************/
void sl_init(){
	printf("initing..\n");
   // glob_ctl = new Glob_Ctl();
    GLOB_NODE_INIT = 1;
    glob_sig = new Sig();
    init_globals();
}

void call_sig(){
    glob_sig->callChildren();
}

#endif //SOUNDLIB_H