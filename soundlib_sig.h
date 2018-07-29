#ifndef SOUNDLIB_SIG_H
#define SOUNDLIB_SIG_H

#include "soundlib.h"

/**** sin osc ****/
class Osc : public Sig{

	double phase = 0, step = 0;
	float* table;
	unsigned int tsize; 

public:

    Osc(){
        table = sinetable;
        tsize = table_size;
    }
    Osc(double freq){
        table = sinetable;
        tsize = table_size;
        step = freq*step_rate;
    }
   ~Osc(){}
 
    float out(double freq){
        phase += freq*step_rate;
        if(phase >= tsize){ phase -= tsize; }       
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);       
    }
   
    float out(){
        phase += step;
        if(phase >= tsize){ phase -= tsize; }       
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);       
   }

};

/**** ASDR env ****/
class Adsr : public Env{
    
    bool attack = true;
    float s = 0.7;
    float astep = 0.0001;
    float dstep = 0.0001;
    float rstep = 0.0001;

public:

    void setA(float ms){
      astep = 1/(sampling_rate*ms*0.001);
    }
    void setD(float ms){
      dstep = 1/(sampling_rate*ms*0.001);
    }
    void setS(float _lev){
        s = _lev;
        if(s > 1){s = 1;}
        if(s < 0){s = 0;}
    }
    void setR(float ms){
      rstep = 1/(sampling_rate*ms*0.001);
    }
    
    void setADSR(float _a, float _d, float _s, float _r){
        setA(_a); setD(_d); setS(_s); setR(_r);
    }

    float out(unsigned int trig){
        if(trig)
        {  on = 1;
           if(attack){
              if(output < 1.0){output += astep;}else{ attack = false; }         
           }
           else{
              if(output > s){output -= dstep;} 
           }
        }
        else{
            attack = true;
            if(output > 0){output -= rstep;}else{ on = 0; }
        }
        
        if(output > 1){output = 1;}
        if(output < 0){output = 0;}

        return output;
    }

    void reset(){
        output = 0;
        on = 0;
        attack = true;
    }

};


/**** Voice base class ****\
class Voice : public Sig, public Ctl{
public:
    virtual float out(float freq, int trig){ return 0; } 
    virtual float out(int note, int trig){ return 0; }
    virtual float out(Note note){ return 0; }
    virtual float out(){ return 0; }
    virtual Env* getEnv(){return NULL;}

};
*/

class TestVoice : public Voice{

    Osc osc;
    Adsr env;
    unsigned int on;
    float hz;
    Env* env_ptr;
public:

    TestVoice(){
        msg_alloc(1);
        env_ptr = &env;
    }

    void run(Msg _m){
        m.value[0]._n = _m.value[0]._n;
        hz = mtof(m.value[0]._n.note);
        on = m.value[0]._n.on;
    }

    float out(){
        return osc.out(hz)*env.out(on);
    }

    Env* getEnv(){
        return env_ptr;
    }

};


#endif /* SOUNDLIB_SIG_H */