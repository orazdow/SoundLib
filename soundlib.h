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

/**** Msg struct ****/
struct Note{
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
        // do stuff, alter m..
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
    virtual float out(double freq){ return 0;}
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
};

/**** PolyVoice base ****/
class PolyVoice :  public Sig, public Ctl{
public:
    int num;
    virtual float out(int note, int trig){ return 0; }
    virtual float out(Note note){ return 0; }
    virtual float out(){ return 0; }
    virtual Env** getEnvs(){return NULL;}    
};

/***** Ctl root class *****/
class Glob_Ctl : public Ctl{
public:
    Glob_Ctl() : Ctl(1){}
    void run(){ callChildren(m); }
};

/********  init  *************/
void sl_init(){
    glob_ctl = new Glob_Ctl();
    init_globals();
}

void call_ctl(){
    glob_ctl->run();
}


#endif /* SOUNDLIB_H */