#ifndef SOUNDLIB_H
#define SOUNDLIB_H

#define static_build 0

#pragma comment (lib, "user32.lib")

#if static_build
    #pragma comment(lib, "advapi32.lib")
    #pragma comment(lib, "ole32.lib")
    #pragma comment(lib, "winmm.lib")
#endif

#include <stdint.h>
#include <map>
#include <stdio.h>
#include "soundlib_glob.h"

/************
need: cycle detection
************/

/*************
[1 s s s n n n n ][0 x x x x x x x ][0 y y y y y y y]
msb:1=status 0=data, sss=masg type, nnnn=channel 
Message         Status      Data 1          Data 2
Note off        8n          Note number     Velocity
Note on         9n          Note number     Velocity
poly aftertouch An          Note number     Pressure
Control change  Bn          Control number  Data
Program change  Cn          Program number  -
Ch aftertouch   Dn          Pressure        -
Pitch wheel     En          LSbyte          MSbyte 
**************/

/**** Msg struct ****/
struct Note{ // is_msg......
 unsigned short note;
 uint8_t vel;
 bool on;
 short pitch;
};

union Val{
    uint32_t _i;
    float _f; 
    Note _n;
};

struct Msg{
    Val* value;
    size_t num;
    int index;
};


/*** Ctl base class ***/
class Ctl{

public:
    Msg m = {0,0,0};
    unsigned int id; 
    std::map <int, Ctl*> childs;
    Ctl* parent;

    Ctl(int _master = 0){
        if(_master)
        if(!GLOB_SET){ 
            master = 1;
            GLOB_SET = 1;
        }
        this->id = ++g_id;
        if(!_master){
            glob_ctl->connect(this);
        }
    }

    Ctl(const Ctl& obj){
        m.num = obj.m.num;
        m.value = new Val[obj.m.num];
        for(int i = 0; i < m.num; i++){
            m.value[i] = obj.m.value[i];
        }
    }

    virtual ~Ctl(){ 
        delete[] m.value;
    }

    // user override
    virtual void run(){}
    virtual void run(Msg _m){}
    virtual void onConnect(Ctl* child){}

    void connect(Ctl* child){ 
        childs[child->id] = child;
        child->parent = this;
        if(!master)
            glob_ctl->disconnect(child);
        onConnect(child);
    }
    
    void disconnect(Ctl* child){
        childs.erase(child->id);
    }
    
protected:
    int master = 0;
    
    void call(Msg _m){ 
        // process msg..
        if(_m.num){ 
            run(_m);
        }else{
            run(); 
        }
        callChildren(m);
    }

    // index always added
    void callChildren(Msg _m){ 
        int i = 0;
        for(auto p : childs){
            _m.index = i++;
            p.second->call(_m);
        }
    }

    void msg_alloc(size_t num){ 
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

/**** Sig base ****/
class Sig{ 
public:
    float* input;
    float  output;
    virtual float out(){ return 0;}
    virtual float out(double in){ return 0;}
    virtual void connect(Sig* sig){ sig->input = &output;  onConnect(sig); }
    virtual void disconnect(Sig* sig){ sig->input = NULL; }
    virtual void onConnect(Sig* sig){}
};


/**** Sig chain base ****/
class SigChain : public Sig{

public:
    unsigned int id; 
    std::map <int, SigChain*> childs;

    SigChain(int _master = 0){
        if(_master)
        if(!GLOB_SIG_SET){ 
            master = 1;
            GLOB_SIG_SET = 1;
        }
        this->id = ++g_id;  
        if(!_master){
            glob_sig->connect(this); 
        }
    }

    virtual void dsp(){}
    
    void connect(SigChain* sig){ 
        sig->input = &output;
        childs[sig->id] = sig;
            if(!master)
                glob_sig->disconnect(sig);          
    }
    void disconnect(SigChain* sig){ 
        sig->input = NULL;
        childs.erase(sig->id); 
    }

protected:
    int master = 0;
    void call(){ 
        dsp();
        callChildren();     
    }

    void callChildren(){ 
        for(auto p : childs){
            p.second->call();   
        }
    } 

};

/***** Env base *****/
class Env : public Sig{
public: 
   unsigned int on = 0; 
   virtual float out(unsigned int trig){ return 0;}
   virtual void reset(){}
};

/**** Voice base ****/
class Voice : public Sig, public Ctl{
public:
    virtual float out(float freq, int trig){ return 0; } 
    virtual float out(int note, int trig){ return 0; }
    virtual float out(Note note){ return 0; }
    virtual float out(){ return 0; }  
    virtual Env* getEnv(){return NULL;}

    void connect(Sig* s){Sig::connect(s);}
    void disconnect(Sig* s){Sig::disconnect(s);}  
};

/**** PolyVoice base ****/
class PolyVoice :  public Sig, public Ctl{
public:
    int num;
    virtual float out(int note, int trig){ return 0; }
    virtual float out(Note note){ return 0; }
    virtual float out(){ return 0; }
    virtual Env** getEnvs(){return NULL;}  
    
    void connect(Sig* s){Sig::connect(s);}
    void disconnect(Sig* s){Sig::disconnect(s);}  
};

/***** Ctl root class *****/
class Glob_Ctl : public Ctl{
public:
    Glob_Ctl() : Ctl(1){}
    void run(){ callChildren(m); }
};

/***** Sig root class *****/
class Glob_Sig : public SigChain{
public:
    Glob_Sig() : SigChain(1){}
    void dsp(){ callChildren(); }
};

/********  init  *************/
void sl_init(){
    glob_ctl = new Glob_Ctl();
    glob_sig = new Glob_Sig();
    init_globals();
}

void call_ctl(){
    glob_ctl->run();
}

void call_sig(){
    glob_sig->dsp();
}
#endif /* SOUNDLIB_H */