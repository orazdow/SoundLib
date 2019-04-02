#ifndef SOUNDLIB_H
#define SOUNDLIB_H

#include "soundlib_glob.h"
#include "map.h"

/*
    
    alternate connect scheme using dynamic cast and virtual base class
    -don't have to inherit Dual
    -setContext() doesn't work...   
*/

void sig_connect(Sig* a, Sig* b, uint inlet = 0);
void sig_disconnect(Sig* a, Sig* b, uint inlet = 0);
void ctl_connect(Ctl* a, Ctl* b);
void ctl_disconnect(Ctl* a, Ctl* b);

class Node{
public:

    void connect(Node* b, uint inlet = 0);
    void disconnect(Node* b, uint inlet = 0);

    uint8_t node_type = 0;

    virtual ~Node(){}

};


/************ Sig ************/

class Sig : public virtual Node{ 
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
        node_type |= TYPE_SIG;
        _init((float)val, num_inlets, num_summing, map_limit);

        if(!SWITCH_CHAIN_INDEPENDENT){
            if(!master)
            glob_sig->connect(this); 
        }else{ 
            if(sig_context){ 
                sig_context->connect(this); 
            }
         //   SWITCH_CHAIN_INDEPENDENT = 0;
        }

    }

    Sig() : Sig(0){ } 
    Sig(bool summing, uint inlets): Sig(0){ init(summing, inlets); } 
    virtual ~Sig(){ delete[] inputs; delete input_bus; delete child_map; }

    virtual void dsp(){ output = *input; } 
    virtual float out(){ return output = *input; }
    virtual float out(double in){ return output = (float)in; }

    // init num inputs. summing status
    void init(uint _inlets = 1, bool _summing = auto_summing){
        summing = _summing;
        inlets = _inlets;
    }

    // set temp val: *val, inlet (index from 0)
    void initVal(float* val, int inlet){
        input_bus->update(val, inlet);
    }


    virtual void bypass_summing(uint inlet = 0){
        inputs[inlet] = input_bus->inputs[inlet*num_summing];
        input = inputs[0];        
    }

    // not making virtual (to save ...cycles..)
    inline void sumInputs(){ 
        for(int i = 0; i < inlets; i++) 
            input_bus->sum(i);
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

void sig_connect(float* a, Sig* b, uint inlet = 0){
    b->input_bus->add(a, (std::uintptr_t)a, inlet);
}

/************ Ctl ************/

/**** Msg struct ****/
struct Note{ // is_msg......
    unsigned short note; //60
    uint8_t vel; //127
    bool on; //0
    short pitch; //0
};

union Val{
    uint32_t _i;
    float _f; 
    Note _n;
};

struct Msg{
    Val* value;
    uint num = 0;
    uint index = 0;
    uint8_t type = 0;
};


class Ctl : public virtual Node{

public:

    uint id;
    bool master = 0;
    Msg m = {0,0,0};
    Ctl* parents;
    Node_map<Ctl>* child_map;
    Node_map<Ctl>* parent_map;
    friend void call_ctl();

    Ctl(bool init_indep = 0){
        if(GLOB_NODE_INIT){
            master = 1;
            GLOB_NODE_INIT = 0;
        }
        this->id = g_id++;
        node_type |= TYPE_CTL;
        child_map = new Node_map<Ctl>(map_limit);
        parent_map = new Node_map<Ctl>(map_limit);
        if(!(SWITCH_CHAIN_INDEPENDENT || init_indep)){
            if(!master)
            glob_ctl->connect(this); 
        }else{
            if(ctl_context){ ctl_context->connect(this); }
           //  SWITCH_CHAIN_INDEPENDENT = 0;
        }
        
    }   

    virtual ~Ctl(){ delete[] m.value; delete child_map; delete parent_map; }

    // user override
    virtual void run(){}
    virtual void run(Msg _m){}
    virtual void onConnect(Ctl* child){}

protected:

    void call(Msg _m){ 
        // process msg..
        if(_m.num){ 
            run(_m);
        }else{
            run(); 
        }
        callChildren();
    }

    // index always added
    void callChildren(){ 
        for(int i = 0; i < child_map->addptr; i++){
            m.index = MIN(i, m.num-1);
            child_map->nodes[i]->call(m);
        }
    }

    void msg_alloc(size_t num){ //should be realloc...
         m.num = num;
         m.value = new Val[num]();
    }    
    // convenience func
    void copy_msg(Msg _m){
        for(int i = 0; i < m.num; i++){
            m.value[i] = _m.value[i];
        }
    }    
};


void ctl_connect(Ctl* a, Ctl* b){ 
    a->child_map->add(b, b->id);
    b->parent_map->add(a, a->id);
    if(!a->master)
        glob_ctl->disconnect(b);
    a->onConnect(b);
}

void ctl_disconnect(Ctl* a, Ctl* b){ 
    a->child_map->remove(b->id);
    b->parent_map->remove(a->id);
}


void Node::connect(Node* b, uint inlet){

    if((b->node_type == TYPE_SIG || b->node_type == TYPE_DUAL) && node_type != TYPE_CTL){
        sig_connect(dynamic_cast<Sig*>(this), dynamic_cast<Sig*>(b), inlet);
    }

    if((b->node_type == TYPE_CTL || b->node_type == TYPE_DUAL) && node_type != TYPE_SIG){
        ctl_connect(dynamic_cast<Ctl*>(this), dynamic_cast<Ctl*>(b));
    }

}

void Node::disconnect(Node* b, uint inlet){
    if((b->node_type == TYPE_SIG || b->node_type == TYPE_DUAL) && node_type != TYPE_CTL){
        sig_disconnect(dynamic_cast<Sig*>(this), dynamic_cast<Sig*>(b), inlet);
    }

    if((b->node_type == TYPE_CTL || b->node_type == TYPE_DUAL) && node_type != TYPE_SIG){
        ctl_disconnect(dynamic_cast<Ctl*>(this), dynamic_cast<Ctl*>(b));
    }
}



// class Dual : public Sig, public Ctl{
// public:

//     void connect(Sig* s){Sig::connect(s);}
//     void disconnect(Sig* s){Sig::disconnect(s);}  
//     void connect(Ctl* c){Ctl::connect(c);}
//     void disconnect(Ctl* c){Ctl::disconnect(c);}  
//     void connect(Dual* d){Sig::connect((Sig*)d); Ctl::connect((Ctl*)d);}
//     void disconnect(Dual* d){Sig::connect((Sig*)d); Ctl::disconnect((Ctl*)d);}   
    
// };


/***** Env base *****/
class Env : public Sig, public Ctl{
public: 
   // Env() : Ctl(chain_independent){}
    uint on = 0; 
    virtual float out(unsigned int trig){ return 0;}
    virtual void reset(){}
    void setTrig(uint trig){ on = trig; }
};

/**** Voice base ****/
class Voice : public virtual Sig, public virtual Ctl{
public:
    Voice(){
        msg_alloc(1);
    }
    virtual float out(float freq, int trig){ return 0; } 
    virtual float out(int note, int trig){ return 0; }
    virtual float out(Note note){ return 0; }
    virtual float out(){ return 0; }  
     // ..............................

};

/**** PolyVoice base ****/
class PolyVoice : public Sig, public Ctl{
public:
    int num; // ctor
    virtual float out(int note, int trig){ return 0; }
    virtual float out(Note note){ return 0; }
    virtual float out(){ return 0; }
    virtual Env** getEnvs(){return NULL;}  
    


};


void set_context(Dual* v){ 
    ctl_context = (Ctl*)v;
    sig_context = (Sig*)v;
    SWITCH_CHAIN_INDEPENDENT = 1;
}
void set_context(Voice* v){ 
    ctl_context = (Ctl*)v;
    sig_context = (Sig*)v;
    SWITCH_CHAIN_INDEPENDENT = 1;
}
 void set_context(std::nullptr_t){
    ctl_context = nullptr;
    sig_context = nullptr;
    SWITCH_CHAIN_INDEPENDENT = 1;
}
void end_context(){
    ctl_context = nullptr;
    sig_context = nullptr;
    SWITCH_CHAIN_INDEPENDENT = 0;
}

/****** independent keyword *******/ //not using..
void* operator new (size_t size, std::nullptr_t) { 
    set_chain_independent(1);
    void* rtn = ::operator new(size);
    return rtn;
}
// Type* obj = independent Type()
#define independent new(nullptr)

/********  init  *************/
void sl_init(){
    GLOB_NODE_INIT = 1;
    glob_ctl = new Ctl();
    GLOB_NODE_INIT = 1;
    glob_sig = new Sig();
    init_globals();
}

void call_ctl(){
    glob_ctl->callChildren();
}

void call_sig(){
    glob_sig->callChildren();
}

#endif //SOUNDLIB_H