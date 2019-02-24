#ifndef SOUNDLIB_GLOB_H
#define SOUNDLIB_GLOB_H

#include <math.h>
// #include "lib/osio.h"

typedef unsigned int uint;

#define auto_summing 1
#define num_inlets 8
#define num_summing 64

#define map_limit 512 //...
#define inc_limit(a, limit) a = (a < limit) ? a+1 : a
#define dec_limit(a, limit) a = (a > limit) ? a-1 : a

class Ctl;
class Sig;

static uint g_id = 0;
static float _null = 0;

static bool GLOB_SET = 0;
static bool GLOB_SIG_SET = 0;
static bool SWITCH_CHAIN_INDEPENDENT = 0;

static Ctl* glob_ctl;
static Sig* glob_sig;

static unsigned int sampling_rate = 44100;
static const unsigned int table_size = 512;
static double step_rate = table_size/(double)sampling_rate;
// step = freq*stepRate

static const double pi = acos(0)*2;
static const double tau = 2*pi;

// namespace constants
namespace sl{
	static const int sin = 0, cos = 1, ramp = 2, saw = 2, tri = 3, square = 4; 
}

static float sinetable[table_size+1];
static double notes[128];
unsigned static int keymap_s[21];


void initSineTable(float table[], int tsize, bool guard_point){
    for (int i = 0; i < tsize; i++) {
         table[i] =  sin(tau*(i/(double)tsize)); 
       // table[i] = tanh(sin(tau*(i/(double)tsize))*2.0);
    }
    if(guard_point)
        table[tsize] = table[0];
}

static double numToFreq(int n){
     return 130.813*pow(2, (n-48)/(double)12);
}

void fillNotes(double notes[], int len, int tablesize, double samplerate){
    for(int i = 0; i < len; i++){
     notes[i] = numToFreq(i);
    }
}

/**** mtof ****/
static inline double mtof(int note){
	if(note < 0){ note = 0;}
	else if(note > 127){ note = 127;}
	return notes[note];
}

/**** lerp ****/
static inline double lerp(double v0, double v1, double t) {
  double fracpart = t-(int)t;
  return v0 + fracpart * (v1 - v0);
} 

static inline double crossfade(double a, double b, double ratio){
    return a*ratio + b*(1.0-ratio);
}

void init_keymap(){
    keymap_s[0] = 'A';
    keymap_s[1] = 'Z';
    keymap_s[2] = 'S';
    keymap_s[3] = 'X';
    keymap_s[4] = 'D';
    keymap_s[5] = 'C';
    keymap_s[6] = 'F';
    keymap_s[7] = 'V';
    keymap_s[8] = 'G';
    keymap_s[9] = 'B';
    keymap_s[10] = 'H';
    keymap_s[11] = 'N';
    keymap_s[12] = 'J';
    keymap_s[13] = 'M';
    keymap_s[14] = 'K';
    keymap_s[15] = 188;
    keymap_s[16] = 'L';
    keymap_s[17] = 190;
    keymap_s[18] = 186;
    keymap_s[19] = 191;
    keymap_s[20] = 222;
}

void init_globals(){
	init_keymap();
	initSineTable(sinetable, table_size, true);
    fillNotes(notes, 128, table_size, sampling_rate);
   // initConsole();
};

#endif /* SOUNDLIB_GLOB_H */