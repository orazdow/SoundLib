#ifndef SOUNDLIB_SIG_H
#define SOUNDLIB_SIG_H

#include "soundlib.h"

class Osc : public Sig{
   const double rate = 1/44100.0;
   const double tau = acos(-1)*2;
   double step = 0;
public:
    Osc(){}
    Osc(double val) : Sig(val){}

    void dsp(){
        step += tau * *input * rate;
        if(step >= tau){step -= tau;}
        output = sin(step);
    }
};


/* single inlet sum/mult node*/
class Bus : public Sig{

public:

    uint op = sl::sum;

    Bus() : op(sl::sum){
        init(false); // <if sumInputs is not virtual
    }

    Bus(uint _op) : op(_op){
        init(false);
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
        init(true, 2);
        val2 = f;
        input_bus->update(&val2, 1);
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
        init(true, 2);
        val2 = f;
        input_bus->update(&val2, 1);        
    }

    Mult() : Mult(1){}

    void dsp(){
        output = *input;
        output *= *inputs[1];
    }

};


#endif //SOUNDLIB_SIG_H