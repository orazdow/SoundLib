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
    bool summing = auto_summing; 
    uint inlets = 1; 

    float* input;
    float** inputs;
    SigBus* input_bus;
    float val = 0;
    float output = 0;
    //std::map <int, Sig*> childs;
    Node_map<Sig>* child_map;
    friend void call_sig();

    Sig(double val){ 
        if(GLOB_NODE_INIT){
            master = 1;
            GLOB_NODE_INIT = 0;
        }
        this->id = g_id++;  
        _init((float)val, num_inlets, num_summing, map_limit);

        if(!master && !SWITCH_CHAIN_INDEPENDENT){
           glob_sig->connect(this); 
        }
    }

    Sig() : Sig(0){ } 
    Sig(bool summing, uint inlets): Sig(0){ init(summing, inlets); } 
    ~Sig(){ delete[] inputs; delete input_bus; }

    virtual void dsp(){ output = *input; } 
    virtual float out(){ return output = *input; }
    virtual float out(double in){ return output = (float)in; }

    // return ptr...
    void connect(Sig* child, uint inlet = 0){ sig_connect(this, child, inlet);}
    void disconnect(Sig* child, uint inlet = 0){ sig_disconnect(this, child, inlet);}

    virtual void bypass_summing(uint inlet = 0){
        inputs[inlet] = input_bus->inputs[inlet*num_summing];
        input = inputs[0];        
    }

private:

    void _init(float f, uint n_inlets, uint n_summing, uint n_map){
        inputs = new float*[n_inlets];
        input_bus  = new SigBus(n_inlets, n_summing);
        child_map = new Node_map<Sig>(n_map); // not needed if using std:map
        for(int i = 0; i < n_inlets; i++)
                inputs[i] = &input_bus->outputs[i];
        
        input = inputs[0]; 
        val = f;
        input_bus->update(&val, 0); 
    }

protected:

    void init(bool _summing = 1, uint _inlets = 1){
        summing = _summing;
        inlets = _inlets;
    }

    // not making overridable (to save cycles..?)
    inline void sumInputs(){ 
        for(int i = 0; i < inlets; i++) 
            input_bus->sum(i);
    }

    void call(){ 
        if(summing)
            sumInputs(); 
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

    if(!b->parents && inlet == 0){ // < condition not needed for sorted queue (connect everything)
       // a->childs[b->id] = b;
        a->child_map->add(b, b->id);
        b->parents++;
    }

    if(!a->master){
        b->input_bus->add(&a->output, a->id, inlet);
        if(!b->summing) b->bypass_summing(inlet);
    }

}


void sig_disconnect(Sig* a, Sig* b, uint inlet){
    
    //a->childs.erase(b->id);
    if(a->child_map->remove(b->id)) 
        dec_limit(b->parents, 0);

    if(!b->parents && !a->master){ glob_sig->connect(b); }    

    if(!a->master){
        b->input_bus->remove(a->id, inlet); 
        if(!b->summing) b->bypass_summing(inlet);
    }

}


/********  init  *************/
void sl_init(){
   // glob_ctl = new Glob_Ctl();
    GLOB_NODE_INIT = 1;
    glob_sig = new Sig();
    init_globals();
}

void call_sig(){
    glob_sig->callChildren();
}

#endif //SOUNDLIB_H