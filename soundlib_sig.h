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


class Sum : public Sig{
   
public:
    Sum(){
        init(true);
    }

    void dsp(){ 
         output =  *input;
    }

};

class Mult : public Sig{
   
public:
    Mult(){
       init(true);
    }

    void sumInputs(){ 
       input_bus->mult(0);
    }

    void dsp(){
        output =  *input;
    }

};


#endif //SOUNDLIB_SIG_H