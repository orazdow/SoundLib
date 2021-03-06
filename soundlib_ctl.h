#ifndef SOUNDLIB_CTL_H
#define SOUNDLIB_CTL_H

#include "soundlib.h"
#include "lib/osio.h"


/******
all Ctl must call msg_alloc
******/


/***** keys *****/
class Key : public Ctl{

    int range = 21;
    unsigned int* keymap = keymap_s; 

public:
    enum keylayout {semitones, keys};

    Key(){ 
        range = sizeof(keymap_s)/sizeof(keymap_s[0]);
        msg_alloc(range);
    }

    Key(int layout){
        if(layout == 0){
            keymap = keymap_s;
            range = sizeof(keymap_s)/sizeof(keymap_s[0]);
            msg_alloc(range);
        }else if(layout == 1){
            keymap = keymap_s;
            range = sizeof(keymap_s)/sizeof(keymap_s[0]);
            msg_alloc(range);            
        }
    }

    void run(){ 
        for(int i = 0; i < range; i++){
            m.value[i]._i = getKeyState(keymap[i]);        
        }   
    }

};

/**** mouse ****/
class Mouse : public Ctl{    

public:
    unsigned int width, height;
    float x, y;

    Mouse(){
        width = getScreenWidth();
        height = getScreenHeight();
        msg_alloc(2);
    }

    void run(){
       getCursorPos();
       m.value[0]._f = x = mouseX/(float)width;
       m.value[1]._f = y = mouseY/(float)height;
    }

    float getX(){return m.value[0]._f;}
    float getY(){return m.value[1]._f;}

};

/***** poly handler *****/
class Poly : public Ctl{

public:   
    int polylimit = 8;
    int baseNote = 48;
    
    struct key{
        int note = 0;
        bool on = 0;  
        bool envset = 0;
        Env* env;
        bool ready(){ 
            return !(on || (envset && env->on));
        }
        void setEnv(Env* e){ 
           env = e; 
           envset = 1; 
       }
    };

    key* keys;
    
    Poly(){
        init();
    }

    Poly(unsigned int poly){
        polylimit = poly;
        init();
    }
    // copy constructor
    Poly(const Poly& obj) : Ctl(obj){
        polylimit = obj.polylimit;
        keys = new key[polylimit];
        for(int i = 0; i < polylimit; i++){
            keys[i] = obj.keys[i];
        }
    }  
    
     ~Poly(){ 
         delete[] keys;
    }

    void setEnvs(Env** envs, int num){
        if(envs)
        for(int i = 0; i < num; i++){
            if(i < polylimit)
                if(envs[i]){
                keys[i].setEnv(envs[i]); 
            }
        }
    }

    void setEnvs(PolyVoice* p){
        setEnvs(p->getEnvs(), p->num);
    }

    void onConnect(Ctl* parent, Ctl* child){
        if(child != NULL)
        if(dynamic_cast<PolyVoice*>(child)){
            setEnvs((PolyVoice*)child);
        }
    }

    void setKey(int note, int on){
        //found:
        for(int i = 0; i < polylimit; i++){
            if(keys[i].note == note){
                keys[i].on = on;
                m.value[i]._n.note = note;
                m.value[i]._n.on = on;
                return;
            }
        } //not found:
        if(on){
            for(int i = 0; i < polylimit; i++){
                 if(keys[i].ready()){
                    keys[i].note = note;
                    keys[i].on = on;
                    m.value[i]._n.note = note;
                    m.value[i]._n.on = on;
                    return;               
                 }
            }//full: (replace with queue to steal oldest)
            keys[0].note = note;
            keys[0].on = on;
            m.value[0]._n.note = note;
            m.value[0]._n.on = on;
        }

    }

    void run(Msg _m){
        for(int i = 0; i < _m.num; i++){
            setKey(i+baseNote, _m.value[i]._i);
        }
    }

    void setBaseNote(unsigned int note){
        baseNote = note;
    }

private:

    void init(){
        keys = new key[polylimit];
        msg_alloc(polylimit);
        for (int i = 0; i < polylimit; i++) {
            keys[i].note = 0;
            keys[i].on = false;
        }
    }

};

/**** combined Key->Poly ****/
class PolyKey : public Poly{
    int range = 21;
    unsigned int* keymap = keymap_s; 
public:

    PolyKey(){
        range = sizeof(keymap_s)/sizeof(keymap_s[0]);
    }

    PolyKey(unsigned int poly) : Poly(poly){
        range = sizeof(keymap_s)/sizeof(keymap_s[0]);
    }

    void run(Msg _m){
        for(int i = 0; i < range; i++){
            setKey(i+baseNote, getKeyState(keymap[i]));
        }
    }
    void run(){
        for(int i = 0; i < range; i++){
            setKey(i+baseNote, getKeyState(keymap[i]));
        }
    }

};



#endif /* SOUNDLIB_CTL_H */