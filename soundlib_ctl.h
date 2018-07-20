#ifndef SOUNDLIB_CTL_H
#define SOUNDLIB_CTL_H

#include "soundlib.h"
#include "windows.h"

/***** keys *****/
class Key : public Ctl{

    static const int range = 21;
    unsigned int keymap[range];
public:
    Key(){ 
        // put keymap(s) in main header globals..
        msg_alloc(range);
        keymap[0] = 'A';
        keymap[1] = 'Z';
        keymap[2] = 'S';
        keymap[3] = 'X';
        keymap[4] = 'D';
        keymap[5] = 'C';
        keymap[6] = 'F';
        keymap[7] = 'V';
        keymap[8] = 'G';
        keymap[9] = 'B';
        keymap[10] = 'H';
        keymap[11] = 'N';
        keymap[12] = 'J';
        keymap[13] = 'M';
        keymap[14] = 'K';
        keymap[15] = 188;
        keymap[16] = 'L';
        keymap[17] = 190;
        keymap[18] = 186;
        keymap[19] = 191;
        keymap[20] = 222;
    }

    inline void run(){ 
        for(int i = 0; i < range; i++){
            m.value[i] = (bool)GetAsyncKeyState(keymap[i]);        
        }   
    }

};

/***** env base *****/
class Env{
public: 
   unsigned int on = 0; 
   virtual double out(unsigned int trig){}
   virtual void reset(){}
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
                *keys[i].val = ((keys[i].note<<16) | on); //set msg val
                break;
            }
        }
        if(!found){
            for(int i = 0; i < polylimit; i++){
                if(keys[i].ready()){    
                    keys[i].note = note;
                    keys[i].on = on;
                    *keys[i].val = ((keys[i].note<<16) | on); //set msg val
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
    // void setPolyLimit(unsigned int poly){
    //     polylimit = poly;
    // }
    
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