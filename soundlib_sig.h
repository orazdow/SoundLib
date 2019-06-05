#ifndef SOUNDLIB_SIG_H
#define SOUNDLIB_SIG_H

#include "soundlib.h"


/* two inlet defaultable sum (as in pd) */
class Sum : public Sig{
public:

    Sum(double f){
        init(2); 
        setVal(f, 1);
    }
    Sum() : Sum(0){}

    void dsp(){ 
        output = *inputs[0];
        output += *inputs[1];
    }

};

/* two inlet defaultable mult (as in pd) */
class Mult : public Sig{
public:

    Mult(double f){
        init(2); 
        setVal(f, 1);   
    }

    Mult() : Mult(1){}

    void dsp(){
        output = *input;
        output *= *inputs[1];
    }

};

/* single inlet sum/mult node*/
class Bus : public Sig{
public:

    uint op = sl::sum;

    Bus() : op(sl::sum){
        init(1, false); // <if sumInputs is not virtual
    }

    Bus(uint _op) : op(_op){
        init(1, false);
    }

    void dsp(){
        // ..in dsp if sumInputs is not virtual
        switch(op){
            case sl::sum : input_bus->sum(0); 
            break;
            case sl::mult : input_bus->mult(0);
        }
        output =  *input;
    }

    virtual void bypass_summing(uint inlet = 0){/* override */}

};

/*
class Osc : public Sig{
   const float rate = 1/(float)sampling_rate;
   const float tau = acos(0)*4;
   float step = 0;
public:
    Osc(float val = 0) : Sig(val){}

    void dsp(){
        step += tau * *input * rate;
        if(step >= tau){step -= tau;}
        output = sinf(step);
    }
};
*/

/**** sin osc ****/
class Osc : public Sig{
    double phase = 0;
    float* table;
public:
    Osc(){
        table = sinetable;
    }
    Osc(double freq) : Sig(freq){
        table = sinetable;
    }
   ~Osc(){} 

    void dsp(){
        phase += *input * table_rate;
        if(phase >= table_size){phase -= table_size;}
        if(phase < 0 ){phase += table_size;}
        output = lerp(table[(int)phase], table[(int)phase+1], phase);  
    }

    float out(double freq){
        phase += freq * table_rate;
        if(phase >= table_size){phase -= table_size;}
        if(phase < 0 ){phase += table_size;}
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);  
    }  
    float out(){
        phase += *input * table_rate;
        if(phase >= table_size){phase -= table_size;}
        if(phase < 0 ){phase += table_size;}
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);        
   }
};

/**** function gen ****/
// -add ramp,  phase
class FnGen : public Sig{

    double phase = 0;
    double rate = 1/(double)sampling_rate;
    float (FnGen::*func)(double freq) = &FnGen::sin;

public:

    FnGen(){}
    FnGen(int wave){ setWave(wave); }
    FnGen(int wave, double freq) : Sig(freq){ setWave(wave); }

    // waves: sl::sin, sl::cos, sl::saw, sl::tri, sl::square 
    void setWave(int a){ 
        switch(a){
            case 0: func = &FnGen::sin; break;
            case 1: func = &FnGen::cos; break;
            case 2: func = &FnGen::saw; break;
            case 3: func = &FnGen::tri; break;
            case 4: func = &FnGen::square; break;
        }
    }

    void dsp(){ 
        (this->*func)(*input); 
    }

    float out(double freq){ 
        return (this->*func)(freq); 
    }
    float out(){ 
        return (this->*func)(*input); 
    }

    float sin(double freq){ 
        phase += freq*tau*rate;
        if(phase > tau){ phase -= tau; }
        if(phase < 0){ phase += tau; }
        return output = std::sin(phase);
    }

    float cos(double freq){
        phase += freq*tau*rate;
        if(phase > tau){ phase -= tau; }
        if(phase < 0){ phase += tau; }
        return output = std::cos(phase);
    }

    float saw(double freq){
        phase += freq*2*rate;
        if(phase >= 1){ phase -= 2;}
        if(phase < -1){ phase += 2;}
        return output = phase;        
    }

    float tri(double freq){
        phase += freq*rate;
        if(phase >= 1){ phase -= 1;}
        if(phase < 0){ phase += 1;}
        if(phase <= 0.5){
        output = phase;
        }else{
        output = 1.0 - phase;
        }
        output = (output-0.25)*4;
        return output;
    }

    float square(double freq){
        phase += freq*rate;
        if(phase >= 1){ phase -= 1;}
        if(phase < 0){ phase += 1;}
        if(phase >= 0.5)
            return output = 1;
        else return output = -1;
    }

};


/**** 1pole lowpass ****/
class Lp : public Sig{

    float param = 0;
    double hz = 0, a = 1, b = 1;
    float* cutoff;

public:

    Lp(){
        init(2);
        cutoff = inputs[1];
    }
    Lp(double freq){
        init(2);
        param = freq;
        initVal(&param, 1);
        cutoff = inputs[1];
    }
   ~Lp(){} 

    inline void dsp(){
        setCutoff(*cutoff);
        output = *input*b + output*a;
    } 

    inline void setCutoff(double hz){
        if(hz < 0) hz = 0;
        b = tau*hz/sampling_rate;
        a = 1.0 - b;
    }

    float filter(double in, double cutoff){
        setCutoff(cutoff);
        output = in*b + output*a;
        //in1 = in;
        return output;
    }

};

/**** ASDR env ****/
class Adsr : public Env{
    
    bool isAttack = true;
    uint trig = 0;

    float attack = 500;
    float sustain = 0.7;
    float decay = 500;
    float release = 500;

    float astep = 0.01;
    float dstep = 0.01;
    float rstep = 0.01;

public:

    Adsr(){
        setADSR(attack, decay, sustain, release);       
    }

    void setAttack(float ms){
        attack = ms;  
        astep = 1/(sampling_rate*ms*0.001);
    }
    void setDecay(float ms){
        decay = ms;
        dstep = 1/(sampling_rate*ms*0.001);
    }
    void setSustain(float _lev){
        sustain = _lev;
        if(sustain > 1){sustain = 1;}
        if(sustain < 0){sustain = 0;}
    }
    void setRelease(float ms){
        release = ms;
        rstep = 1/(sampling_rate*ms*0.001);
    }
    
    void setADSR(float _a, float _d, float _s, float _r){
        setAttack(_a); setDecay(_d); setSustain(_s); setRelease(_r);
    }

    void run(Msg _m){
        trig = _m.value[_m.index]._n.on;
        // printf("HEY%u\n", trig);
    }

    void dsp(){
        out(trig);
    }

    float out(unsigned int trig){ //printf("%u\n", trig);
        if(trig)
        {  on = 1;
           if(isAttack){
              if(output < 1.0){output += astep;}else{ isAttack = false; }         
           }
           else{
              if(output > sustain){output -= dstep;} 
           }
        }
        else{
            isAttack = true;
            if(output > 0){output -= rstep;}else{ on = 0; }
        }
        
        if(output > 1){output = 1;}
        if(output < 0){output = 0;}

        return output;
    }

    void reset(){
        output = 0;
        on = 0;
        isAttack = true;
    }

};

/****** test voice ******/
class TestVoice : public Voice{

public:

    FnGen* osc1;
    FnGen* osc2;
    Adsr* env;
    unsigned int on = 0;
    float hz;
    FnGen* lfo;

    
    TestVoice(){ 
       //  set_chain_independent(true);
        set_context(this);
        osc1 = new FnGen(sl::saw);
        osc2 = new FnGen(sl::saw);
        env = new Adsr();
        lfo = new FnGen(sl::sin);
     //    set_chain_independent(false);
        // connect(osc1);
        // connect(env);
        sig_connect(&hz, osc1);
        end_context();
    }

    ~TestVoice(){
        delete osc1;
        delete osc2;
        delete env;
        delete lfo;
    }

    void run(Msg _m){

        Note n = _m.value[_m.index]._n; 
        on = n.on;
        hz = mtof(n.note);

         copy_msg(_m); 
         //env->run(m); //
        // env->run(_m);  // dsp()

    }

    void dsp(){
      // output = 0.5*(osc1->out(hz)+osc2->out(hz*0.999))*env->out(on);
       // output = osc1->out(hz) * env->out(on);
       //  osc1->dsp();
       // env->dsp();
        output = osc1->output * env->output;
    }

    float out(Note note){
        // hz = mtof(note.note+24)+pow(lfo->out(4.3),1)*4.4; // crossfade((lfo->out(4.3),1)*4.4,1,0.04);
        hz = mtof(note.note);
        on = note.on;
        return 0.5*(osc1->out(hz)+osc2->out(hz*0.999))*env->out(on);
        // return osc1->sin(hz)*env->out(on);
    }

    Env* getEnv(){
        return env;
    }
};



#endif //SOUNDLIB_SIG_H