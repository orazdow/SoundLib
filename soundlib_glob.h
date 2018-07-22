#ifndef SOUNDLIB_GLOB_H
#define SOUNDLIB_GLOB_H

#include <math.h>

class Ctl;
static Ctl* glob_ctl;
static bool GLOB_SET = 0;
static unsigned int g_id = 0;

enum msg_fmt { u32, u32_f, note };

static unsigned int sampling_rate = 44100;
static const unsigned int table_size = 512;

static const double pi = acos(0)*2;
static const double tau = 2*pi;
static double step_rate = table_size/(double)sampling_rate;
// step = freq*stepRate

static float sinetable[table_size+1];
static double notes[128];
unsigned static int keymap_s[21];

/**** lerp ****/
static inline double lerp(double v0, double v1, double t) {
  double fracpart = t-(int)t;
  return v0 + fracpart * (v1 - v0);
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
};

#endif /* SOUNDLIB_GLOB_H */