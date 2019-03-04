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
        if(!auto_summing)
          sumInputs(); 
        // input_bus->sum(0);
         output =  *input;
        //output =  *inputs[0];

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
      if(!auto_summing)
          sumInputs(); 
            //input_bus->mult(0);
        output =  *input;
    }

};


#endif //SOUNDLIB_SIG_H