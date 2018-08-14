
/* globals */
#define TYPE_SSG 0x01    /* SSG support          */
#define TYPE_LFOPAN 0x02 /* OPN type LFO and PAN */
#define TYPE_6CH 0x04    /* FM 6CH / 3CH         */
#define TYPE_DAC 0x08    /* YM2612's DAC device  */
#define TYPE_ADPCM 0x10  /* two ADPCM units      */
#define TYPE_YM2612 (TYPE_DAC | TYPE_LFOPAN | TYPE_6CH)

#define FREQ_SH 16  /* 16.16 fixed point (frequency calculations) */
#define EG_SH 16    /* 16.16 fixed point (envelope generator timing) */
#define LFO_SH 24   /*  8.24 fixed point (LFO calculations)       */
#define TIMER_SH 16 /* 16.16 fixed point (timers calculations)    */

#define FREQ_MASK ((1 << FREQ_SH) - 1)

#define ENV_BITS 10
#define ENV_LEN (1 << ENV_BITS)
#define ENV_STEP (128.0 / ENV_LEN)

#define MAX_ATT_INDEX (ENV_LEN - 1) /* 1023 */
#define MIN_ATT_INDEX (0)           /* 0 */

#define EG_ATT 4
#define EG_DEC 3
#define EG_SUS 2
#define EG_REL 1
#define EG_OFF 0

#define SIN_BITS 10
#define SIN_LEN (1 << SIN_BITS)
#define SIN_MASK (SIN_LEN - 1)

#define TL_RES_LEN (256) /* 8 bits addressing (real chip) */

#define FINAL_SH (0)
#define MAXOUT (+32767)
#define MINOUT (-32768)

/* register number to channel number , slot offset */
#define OPN_CHAN(N) (N & 3)
#define OPN_SLOT(N) ((N >> 2) & 3)

/* slot number */
#define SLOT1 0
#define SLOT2 2
#define SLOT3 1
#define SLOT4 3

/* bit0 = Right enable , bit1 = Left enable */
#define OUTD_RIGHT 1
#define OUTD_LEFT 2
#define OUTD_CENTER 3
