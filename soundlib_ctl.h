#ifndef SOUNDLIB_CTL_H
#define SOUNDLIB_CTL_H

#include "soundlib.h"
#include "windows.h"


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

    inline void run(){ 
        for(int i = 0; i < range; i++){
            m.value[i] = (bool)GetAsyncKeyState(keymap[i]);        
        }   
    }

};

/**** mouse ****/
class Mouse : public Ctl{    
    POINT point;
    // uint32_t floatToUint(float in){
    //     return (uint32_t)(*(uint32_t*)&n);
    // }

public:
    unsigned int width, height;
    double x, y;

    Mouse(){
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        msg_alloc(2);
    }

    void run(){
       GetCursorPos(&point);
       // x = lerp(x, point.x, 0.3);
       // y = lerp(y, point.y, 0.3);
       x = point.x;
       y = point.y;
       m.value[0] = x;
       m.value[1] = y;
    }

};

/***** poly handler *****/
class Poly : public Ctl{

public:   
    int polylimit = 8;
    int baseNote = 48;
    
    typedef struct{
        int note;
        bool on;  
        bool envset = 0;
        Env* env;
        uint32_t* val;

        bool ready(){
            return !( on || (envset && env->on) );
        }
        void setEnv(Env* e){
           env = e; 
           envset = 1;
       }
    }key;

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

    void setKey(int note, int on){
        bool found = false;
        for (int i = 0; i < polylimit; i++){
            if(keys[i].note == note){
                keys[i].on = on;
                found = true;
                // *keys[i].val = ((keys[i].note<<16) | on); //set msg val
                m.value[i] = ((keys[i].note<<16) | on); //set msg val
                break;
            }
        }
        if(!found){
            for(int i = 0; i < polylimit; i++){
                if(keys[i].ready()){    
                    keys[i].note = note;
                    keys[i].on = on;
                    // *keys[i].val = ((keys[i].note<<16) | on); //set msg val
                     m.value[i] = ((keys[i].note<<16) | on); //set msg val
                    break;
                }
            }
        }

    }

    inline void run(Msg _m){
        for(int i = 0; i < _m.num; i++){
            setKey(i+baseNote, _m.value[i]);
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
            keys[i].val = m.value+i;
        }
    }

};


#endif /* SOUNDLIB_CTL_H */