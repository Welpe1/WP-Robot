#ifndef __OLEDFONT_H
#define __OLEDFONT_H

#include <stdint.h>


#define EMO_STATIC_MAX      8
#define EMO_DYNAMICS_MAX    4

#define EMO_LOGO            0
#define EMO_HAPPY_BASE      1
#define EMO_PEACE_BASE      16
#define EMO_SAD_BASE        31
#define EMO_ANGRY_BASE      46
#define EMO_FEAR_BASE       61
#define EMO_SPECIAL_BASE    76


extern const uint8_t emo_data_group[200][128*8];
extern const uint8_t emo_static_group[8][EMO_STATIC_MAX];
extern const uint8_t emo_dynamics_group[5][EMO_DYNAMICS_MAX][9];



#endif
