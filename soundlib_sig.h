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
 
    inline float out(double freq){
        phase += freq*step_rate;
        if(phase >= tsize){ phase -= tsize; }       
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);       
    }
   
    inline float out(){
        phase += step;
        if(phase >= tsize){ phase -= tsize; }       
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);       
   }

};

#endif /* SOUNDLIB_SIG_H */