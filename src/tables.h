#ifndef YM2612_TABLES_H
#define YM2612_TABLES_H

#include <cstdint>
#include "defines.h"

/*  TL_TAB_LEN is calculated as:
*   13 - sinus amplitude bits     (Y axis)
*   2  - sinus sign bit           (Y axis)
*   TL_RES_LEN - sinus resolution (X axis)
*/
#define TL_TAB_LEN (13 * 2 * TL_RES_LEN)
static signed int tl_tab[TL_TAB_LEN];

#define ENV_QUIET (TL_TAB_LEN >> 3)

/* sin waveform table in 'decibel' scale */
static unsigned int sin_tab[SIN_LEN];

/* sustain level table (3dB per step) */
/* bit0, bit1, bit2, bit3, bit4, bit5, bit6 */
/* 1,    2,    4,    8,    16,   32,   64   (value)*/
/* 0.75, 1.5,  3,    6,    12,   24,   48   (dB)*/

/* 0 - 15: 0, 3, 6, 9,12,15,18,21,24,27,30,33,36,39,42,93 (dB)*/
#define SC(db) (uint32_t)(db * (4.0 / ENV_STEP))
static const uint32_t sl_table[16] = {
    SC(0), SC(1), SC(2), SC(3), SC(4), SC(5), SC(6), SC(7),
    SC(8), SC(9), SC(10), SC(11), SC(12), SC(13), SC(14), SC(31)};
#undef SC

static const uint8_t slots_idx[4] = {0,2,1,3};

#define RATE_STEPS (8)
static const uint8_t eg_inc[19 * RATE_STEPS] = {
    /*cycle:0 1  2 3  4 5  6 7*/

    /* 0 */ 0, 1, 0, 1, 0, 1, 0, 1, /* rates 00..11 0 (increment by 0 or 1) */
    /* 1 */ 0, 1, 0, 1, 1, 1, 0, 1, /* rates 00..11 1 */
    /* 2 */ 0, 1, 1, 1, 0, 1, 1, 1, /* rates 00..11 2 */
    /* 3 */ 0, 1, 1, 1, 1, 1, 1, 1, /* rates 00..11 3 */

    /* 4 */ 1, 1, 1, 1, 1, 1, 1, 1, /* rate 12 0 (increment by 1) */
    /* 5 */ 1, 1, 1, 2, 1, 1, 1, 2, /* rate 12 1 */
    /* 6 */ 1, 2, 1, 2, 1, 2, 1, 2, /* rate 12 2 */
    /* 7 */ 1, 2, 2, 2, 1, 2, 2, 2, /* rate 12 3 */

    /* 8 */ 2, 2, 2, 2, 2, 2, 2, 2, /* rate 13 0 (increment by 2) */
    /* 9 */ 2, 2, 2, 4, 2, 2, 2, 4, /* rate 13 1 */
    /*10 */ 2, 4, 2, 4, 2, 4, 2, 4, /* rate 13 2 */
    /*11 */ 2, 4, 4, 4, 2, 4, 4, 4, /* rate 13 3 */

    /*12 */ 4, 4, 4, 4, 4, 4, 4, 4, /* rate 14 0 (increment by 4) */
    /*13 */ 4, 4, 4, 8, 4, 4, 4, 8, /* rate 14 1 */
    /*14 */ 4, 8, 4, 8, 4, 8, 4, 8, /* rate 14 2 */
    /*15 */ 4, 8, 8, 8, 4, 8, 8, 8, /* rate 14 3 */

    /*16 */ 8, 8, 8, 8, 8, 8, 8, 8,         /* rates 15 0, 15 1, 15 2, 15 3 (increment by 8) */
    /*17 */ 16, 16, 16, 16, 16, 16, 16, 16, /* rates 15 2, 15 3 for attack */
    /*18 */ 0, 0, 0, 0, 0, 0, 0, 0,         /* infinity rates for attack and decay(s) */
};

#define O(a) (a * RATE_STEPS)

/*note that there is no O(17) in this table - it's directly in the code */
static const uint8_t eg_rate_select[32 + 64 + 32] = {/* Envelope Generator rates (32 + 64 rates + 32 RKS) */
                                                     /* 32 infinite time rates */
                                                     O(18), O(18), O(18), O(18), O(18), O(18), O(18), O(18),
                                                     O(18), O(18), O(18), O(18), O(18), O(18), O(18), O(18),
                                                     O(18), O(18), O(18), O(18), O(18), O(18), O(18), O(18),
                                                     O(18), O(18), O(18), O(18), O(18), O(18), O(18), O(18),

                                                     /* rates 00-11 */
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),
                                                     O(0), O(1), O(2), O(3),

                                                     /* rate 12 */
                                                     O(4), O(5), O(6), O(7),

                                                     /* rate 13 */
                                                     O(8), O(9), O(10), O(11),

                                                     /* rate 14 */
                                                     O(12), O(13), O(14), O(15),

                                                     /* rate 15 */
                                                     O(16), O(16), O(16), O(16),

                                                     /* 32 dummy rates (same as 15 3) */
                                                     O(16), O(16), O(16), O(16), O(16), O(16), O(16), O(16),
                                                     O(16), O(16), O(16), O(16), O(16), O(16), O(16), O(16),
                                                     O(16), O(16), O(16), O(16), O(16), O(16), O(16), O(16),
                                                     O(16), O(16), O(16), O(16), O(16), O(16), O(16), O(16)

};

#undef O

/*rate  0,    1,    2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15*/
/*shift 11,  10,  9,  8,  7,  6,  5,  4,  3,  2, 1,  0,  0,  0,  0,  0 */
/*mask  2047, 1023, 511, 255, 127, 63, 31, 15, 7,  3, 1,  0,  0,  0,  0,  0 */

#define O(a) (a * 1)
static const uint8_t eg_rate_shift[32 + 64 + 32] = {/* Envelope Generator counter shifts (32 + 64 rates + 32 RKS) */
                                                    /* 32 infinite time rates */
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0),
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0),
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0),
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0),

                                                    /* rates 00-11 */
                                                    O(11), O(11), O(11), O(11),
                                                    O(10), O(10), O(10), O(10),
                                                    O(9), O(9), O(9), O(9),
                                                    O(8), O(8), O(8), O(8),
                                                    O(7), O(7), O(7), O(7),
                                                    O(6), O(6), O(6), O(6),
                                                    O(5), O(5), O(5), O(5),
                                                    O(4), O(4), O(4), O(4),
                                                    O(3), O(3), O(3), O(3),
                                                    O(2), O(2), O(2), O(2),
                                                    O(1), O(1), O(1), O(1),
                                                    O(0), O(0), O(0), O(0),

                                                    /* rate 12 */
                                                    O(0), O(0), O(0), O(0),

                                                    /* rate 13 */
                                                    O(0), O(0), O(0), O(0),

                                                    /* rate 14 */
                                                    O(0), O(0), O(0), O(0),

                                                    /* rate 15 */
                                                    O(0), O(0), O(0), O(0),

                                                    /* 32 dummy rates (same as 15 3) */
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0),
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0),
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0),
                                                    O(0), O(0), O(0), O(0), O(0), O(0), O(0), O(0)

};
#undef O

static const uint8_t dt_tab[4 * 32] = {
    /* this is YM2151 and YM2612 phase increment data (in 10.10 fixed point format)*/
    /* FD=0 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* FD=1 */
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
    2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
    /* FD=2 */
    1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
    5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 16, 16, 16, 16,
    /* FD=3 */
    2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
    8, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 20, 22, 22, 22, 22};

/* OPN key frequency number -> key code follow table */
/* fnum higher 4bit -> keycode lower 2bit */
static const uint8_t opn_fktable[16] = {0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3};

/* 8 LFO speed parameters */
/* each value represents number of samples that one LFO level will last for */
static const uint32_t lfo_samples_per_step[8] = {108, 77, 71, 67, 62, 44, 8, 5};

/*There are 4 different LFO AM depths available, they are:
  0 dB, 1.4 dB, 5.9 dB, 11.8 dB
  Here is how it is generated (in EG steps):

  11.8 dB = 0, 2, 4, 6, 8, 10,12,14,16...126,126,124,122,120,118,....4,2,0
   5.9 dB = 0, 1, 2, 3, 4, 5, 6, 7, 8....63, 63, 62, 61, 60, 59,.....2,1,0
   1.4 dB = 0, 0, 0, 0, 1, 1, 1, 1, 2,...15, 15, 15, 15, 14, 14,.....0,0,0

  (1.4 dB is losing precision as you can see)

  It's implemented as generator from 0..126 with step 2 then a shift
  right N times, where N is:
    8 for 0 dB
    3 for 1.4 dB
    1 for 5.9 dB
    0 for 11.8 dB
*/
static const uint8_t lfo_ams_depth_shift[4] = {8, 3, 1, 0};

/*There are 8 different LFO PM depths available, they are:
  0, 3.4, 6.7, 10, 14, 20, 40, 80 (cents)

  Modulation level at each depth depends on F-NUMBER bits: 4,5,6,7,8,9,10
  (bits 8,9,10 = FNUM MSB from OCT/FNUM register)

  Here we store only first quarter (positive one) of full waveform.
  Full table (lfo_pm_table) containing all 128 waveforms is build
  at run (init) time.

  One value in table below represents 4 (four) basic LFO steps
  (1 PM step = 4 AM steps).

  For example:
   at LFO SPEED=0 (which is 108 samples per basic LFO step)
   one value from "lfo_pm_output" table lasts for 432 consecutive
   samples (4*108=432) and one full LFO waveform cycle lasts for 13824
   samples (32*432=13824; 32 because we store only a quarter of whole
            waveform in the table below)
*/
static const uint8_t lfo_pm_output[7 * 8][8] = {
    /* 7 bits meaningful (of F-NUMBER), 8 LFO output levels per one depth (out of 32), 8 LFO depths */
    /* FNUM BIT 4: 000 0001xxxx */
    /* DEPTH 0 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 1 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 2 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 3 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 4 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 5 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 6 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 7 */ {0, 0, 0, 0, 1, 1, 1, 1},

    /* FNUM BIT 5: 000 0010xxxx */
    /* DEPTH 0 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 1 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 2 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 3 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 4 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 5 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 6 */ {0, 0, 0, 0, 1, 1, 1, 1},
    /* DEPTH 7 */ {0, 0, 1, 1, 2, 2, 2, 3},

    /* FNUM BIT 6: 000 0100xxxx */
    /* DEPTH 0 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 1 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 2 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 3 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 4 */ {0, 0, 0, 0, 0, 0, 0, 1},
    /* DEPTH 5 */ {0, 0, 0, 0, 1, 1, 1, 1},
    /* DEPTH 6 */ {0, 0, 1, 1, 2, 2, 2, 3},
    /* DEPTH 7 */ {0, 0, 2, 3, 4, 4, 5, 6},

    /* FNUM BIT 7: 000 1000xxxx */
    /* DEPTH 0 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 1 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 2 */ {0, 0, 0, 0, 0, 0, 1, 1},
    /* DEPTH 3 */ {0, 0, 0, 0, 1, 1, 1, 1},
    /* DEPTH 4 */ {0, 0, 0, 1, 1, 1, 1, 2},
    /* DEPTH 5 */ {0, 0, 1, 1, 2, 2, 2, 3},
    /* DEPTH 6 */ {0, 0, 2, 3, 4, 4, 5, 6},
    /* DEPTH 7 */ {0, 0, 4, 6, 8, 8, 0xa, 0xc},

    /* FNUM BIT 8: 001 0000xxxx */
    /* DEPTH 0 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 1 */ {0, 0, 0, 0, 1, 1, 1, 1},
    /* DEPTH 2 */ {0, 0, 0, 1, 1, 1, 2, 2},
    /* DEPTH 3 */ {0, 0, 1, 1, 2, 2, 3, 3},
    /* DEPTH 4 */ {0, 0, 1, 2, 2, 2, 3, 4},
    /* DEPTH 5 */ {0, 0, 2, 3, 4, 4, 5, 6},
    /* DEPTH 6 */ {0, 0, 4, 6, 8, 8, 0xa, 0xc},
    /* DEPTH 7 */ {0, 0, 8, 0xc, 0x10, 0x10, 0x14, 0x18},

    /* FNUM BIT 9: 010 0000xxxx */
    /* DEPTH 0 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 1 */ {0, 0, 0, 0, 2, 2, 2, 2},
    /* DEPTH 2 */ {0, 0, 0, 2, 2, 2, 4, 4},
    /* DEPTH 3 */ {0, 0, 2, 2, 4, 4, 6, 6},
    /* DEPTH 4 */ {0, 0, 2, 4, 4, 4, 6, 8},
    /* DEPTH 5 */ {0, 0, 4, 6, 8, 8, 0xa, 0xc},
    /* DEPTH 6 */ {0, 0, 8, 0xc, 0x10, 0x10, 0x14, 0x18},
    /* DEPTH 7 */ {0, 0, 0x10, 0x18, 0x20, 0x20, 0x28, 0x30},

    /* FNUM BIT10: 100 0000xxxx */
    /* DEPTH 0 */ {0, 0, 0, 0, 0, 0, 0, 0},
    /* DEPTH 1 */ {0, 0, 0, 0, 4, 4, 4, 4},
    /* DEPTH 2 */ {0, 0, 0, 4, 4, 4, 8, 8},
    /* DEPTH 3 */ {0, 0, 4, 4, 8, 8, 0xc, 0xc},
    /* DEPTH 4 */ {0, 0, 4, 8, 8, 8, 0xc, 0x10},
    /* DEPTH 5 */ {0, 0, 8, 0xc, 0x10, 0x10, 0x14, 0x18},
    /* DEPTH 6 */ {0, 0, 0x10, 0x18, 0x20, 0x20, 0x28, 0x30},
    /* DEPTH 7 */ {0, 0, 0x20, 0x30, 0x40, 0x40, 0x50, 0x60},

};

/* all 128 LFO PM waveforms */
static int32_t lfo_pm_table[128 * 8 * 32]; /* 128 combinations of 7 bits meaningful (of F-NUMBER), 8 LFO depths, 32 LFO output levels per one depth */




#endif /* YM2612_TABLES_H */
