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


/**** function gen ****/
class FnGen : public Sig{

    double step = 1/(double)sampling_rate;
    double phase, freq;
    float (FnGen::*func)(double freq) = &FnGen::sin;

public:

    FnGen(){}
    FnGen(int wave){ setWave(wave); }
    FnGen(double freq){ this->freq = freq; }

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

    float out(double freq){ 
        return (this->*func)(freq); 
    }
    float out(){ 
        return (this->*func)(freq); 
    }

    float sin(double freq){
        phase += freq*tau*step;
        if(phase > tau){ phase -= tau; }
        return output = std::sin(phase);
    }

    float cos(double freq){
        phase += freq*tau*step;
        if(phase > tau){ phase -= tau; }
        return output = std::cos(phase);
    }

    float saw(double freq){
        phase += freq*2*step;
        if(phase >= 1){ phase -= 2;}
        return output = phase;        
    }

    float tri(double freq){
        phase += freq*step;
        if(phase >= 1){ phase -= 1;}

        if(phase <= 0.5){
        output = phase;
        }else{
        output = 1.0 - phase;
        }
        output = (output-0.25)*4;
        return output;
    }

    float square(double freq){
        phase += freq*step;
        if(phase >= 1){ phase -= 1;}
        if(phase >= 0.5)
            return output = 1;
        else return output = -1;
    }

};

/**** 1pole lowpass ****/
class Lp : public Sig{
    double cutoff = 500, in1 = 1, norm = 1, a = 1, b = 1;
public:
    void setCutoff(double hz){
        cutoff = hz;
        hz *= pi;
        norm = 1.0/(hz+sampling_rate);
        a = hz*norm;
        b = (sampling_rate-hz)*norm;
    }

    float filter(double in, double cutoff){
        setCutoff(cutoff);
        output = in*a + in1*a + output*b;
        in1 = in;
        return output;
    }

    float out(double in){
        output = in*a + in1*a + output*b;
        in1 = in;
        return output;
    } 

    float out(){
        output = *input*a + in1*a + output*b;
        in1 = *input;
        return output;
    } 

    Lp(double hz){ setCutoff(hz);}
    Lp(){};
};


/**** ASDR env ****/
class Adsr : public Env{
    
    bool attack = true;
    float s = 0.7;
    float astep = 0.0001;
    float dstep = 0.0001;
    float rstep = 0.00001;

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


class TestVoice : public Voice{
    FnGen* osc1;
    FnGen* osc2;
    Adsr* env;
    unsigned int on;
    float hz;
    FnGen* lfo;

public:
    
    TestVoice(){ 
        osc1 = new FnGen(sl::saw);
        osc2 = new FnGen(sl::saw);
        env = new Adsr();
        lfo = new FnGen(sl::sin);
    }

    ~TestVoice(){
        delete osc1;
        delete osc2;
        delete env;
        delete lfo;
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

class Synth : public PolyVoice{
    TestVoice* voices;
    Env** envs;
    Lp filt = Lp(200);
    FnGen* lfo;

    void init(int _num){
        num = _num;
        msg_alloc(num);
        voices = new TestVoice[num];
        envs = new Env*[num];   
        for(int i = 0; i < num; i++){
             envs[i] = voices[i].getEnv();  
           //  connect(voices+i);
        }     
        lfo = new FnGen(sl::saw); 
    }

public:
    Synth(){
        init(8);         
    }
    Synth(int _num){
        init(_num);
    }
    ~Synth(){
        delete[] voices;
        delete[] envs;
        delete lfo;
    }

    float out(){
        output = 0;
        for(int i = 0; i < num; i++){ 
            output+= 0.125*voices[i].out(m.value[i]._n);
        }
        return output = filt.filter(output,1500.0*(lfo->out(2)+1.3));
    }

    void run(Msg _m){
       // get notes 
       copy_msg(_m);
    }
        
    Env** getEnvs(){
        return envs;
    }

};

/**** VU meter o _O ****/
class VUMeter : public SigChain, public Ctl{
    int len = 128;
    float* buffer;
    int index = 0;
    float lev, _lev;
    int num;
public:

    VUMeter(){
       buffer = new float[len];
    }

    ~VUMeter(){
        delete[] buffer;
    }

    void run(){
        lev = 0;
        for(int i = 0; i < len; i++){
            lev += pow(buffer[i],2);
        }    
        lev = lev / (float)len;
        lev = sqrt(lev);

        _lev = lerp(_lev, lev*10, 0.3);
        num = (int)(_lev*30);

        if(num > 60) num = 60;
        printf("                                                                 \r");
        for(int i = 0; i < num; i++){
            printf("=");
        }      
    }

    void dsp(){ 
        buffer[index] = *input;
        index = ++index%len;
    }

};

/**** Oscilloscope heh ****/
class Oscilloscope : public SigChain, public Ctl{
    
    float* samples;
    const int len = 60;
    const int ylen = 24;
    const int yl2 = ylen/2;
    // amp >= to ylen
    const int amp = 30;
    // 1-4 are good ranges 
    const int timestep = 3;
    const float dy = 0.5;
    bool trig = false;
    int index = 0;
    int step = 0;
    ConsoleBuffer* cbuffer;
public:

    Oscilloscope(){
        samples = new float[len];
        cbuffer = new ConsoleBuffer(len, ylen);
    }
    ~Oscilloscope(){ delete[] samples;  delete cbuffer; }

    void dsp(){
        if(!trig){
            if(!step){
            samples[index] = *input;
            index = ++index%len; 
            } 
            step = ++step%timestep;
        }       
    }
             
    void run(){  
        trig = true; 
        for(int y = 0; y < ylen; y++){
            for(int x = 0; x < len; x++){  
               if(abs(samples[(x+index)%len]*amp - (ylen-(y+yl2))) < dy){
                    set_buff_char(cbuffer->buffer, y*len+x, '#');
                }else{ 
                    set_buff_char(cbuffer->buffer, y*len+x, ' ');
                }
            }
        } trig = false;
        // setConsolePos(0,0);
        writeConsoleBuffer(cbuffer);
        Sleep(20); //send to buffer in print thread
    } 
};



#endif /* SOUNDLIB_SIG_H */