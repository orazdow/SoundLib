#ifndef SOUNDLIB_SIG_H
#define SOUNDLIB_SIG_H

#include "soundlib.h"

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

/* two inlet defaultable sum (as in pd) */
class Sum : public Sig{

    float val2 = 0;

public:
    Sum(double f){
        val2 = f;
        // 2 inlets
        init(2); 
        // init 2nd inlet
        initVal(&val2, 1);
    }
    Sum() : Sum(0){}

    void dsp(){ 
        output = 0;
        output += *inputs[0];
        output += *inputs[1];
    }

};

/* two inlet defaultable mult (as in pd) */
class Mult : public Sig{
   
   float val2 = 1;

public:

    Mult(double f){
        val2 = f;
        // 2 inlets
        init(2); 
        // init 2nd inlet
        initVal(&val2, 1);       
    }

    Mult() : Mult(1){}

    void dsp(){
        output = *input;
        output *= *inputs[1];
    }

};

/*
class Osc : public Sig{
   const double rate = 1/44100.0;
   const double tau = acos(-1)*2;
   float step = 0;
public:
    Osc(){}
    Osc(double val) : Sig(val){}

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
// -change cycle to start from 0
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

};



#endif //SOUNDLIB_SIG_H