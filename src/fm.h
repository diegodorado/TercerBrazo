#ifndef MAME_SOUND_FM_H
#define MAME_SOUND_FM_H

#include <cstdint>
#include "tables.h"

/* struct describing a single operator (SLOT) */
struct FM_SLOT
{
	int32_t   *DT;        /* detune          :dt_tab[DT] */
	uint8_t   KSR;        /* key scale rate  :3-KSR */
	uint32_t  ar;         /* attack rate  */
	uint32_t  d1r;        /* decay rate   */
	uint32_t  d2r;        /* sustain rate */
	uint32_t  rr;         /* release rate */
	uint8_t   ksr;        /* key scale rate  :kcode>>(3-KSR) */
	uint32_t  mul;        /* multiple        :ML_TABLE[ML] */

	/* Phase Generator */
	uint32_t  phase;      /* phase counter */
	int32_t   Incr;       /* phase step */

	/* Envelope Generator */
	uint8_t   state;      /* phase type */
	uint32_t  tl;         /* total level: TL << 3 */
	int32_t   volume;     /* envelope counter */
	uint32_t  sl;         /* sustain level:sl_table[SL] */
	uint32_t  vol_out;    /* current output from EG circuit (without AM from LFO) */

	uint8_t   eg_sh_ar;   /*  (attack state) */
	uint8_t   eg_sel_ar;  /*  (attack state) */
	uint8_t   eg_sh_d1r;  /*  (decay state) */
	uint8_t   eg_sel_d1r; /*  (decay state) */
	uint8_t   eg_sh_d2r;  /*  (sustain state) */
	uint8_t   eg_sel_d2r; /*  (sustain state) */
	uint8_t   eg_sh_rr;   /*  (release state) */
	uint8_t   eg_sel_rr;  /*  (release state) */

	uint8_t   ssg;        /* SSG-EG waveform */
	uint8_t   ssgn;       /* SSG-EG negated output */

	uint32_t  key;        /* 0=last key was KEY OFF, 1=KEY ON */

	/* LFO */
	uint32_t  AMmask;     /* AM enable flag */

};

struct FM_CH
{
	FM_SLOT SLOT[4];    /* four SLOTs (operators) */

	uint8_t   ALGO;       /* algorithm */
	uint8_t   FB;         /* feedback shift */
	int32_t   op1_out[2]; /* op1 output for feedback */

	int32_t   *connect1;  /* SLOT1 output pointer */
	int32_t   *connect3;  /* SLOT3 output pointer */
	int32_t   *connect2;  /* SLOT2 output pointer */
	int32_t   *connect4;  /* SLOT4 output pointer */

	int32_t   *mem_connect;/* where to put the delayed sample (MEM) */
	int32_t   mem_value;  /* delayed sample (MEM) value */

	int32_t   pms;        /* channel PMS */
	uint8_t   ams;        /* channel AMS */

	uint32_t  fc;         /* fnum,blk:adjusted to sample rate */
	uint8_t   kcode;      /* key code:                        */
	uint32_t  block_fnum; /* current blk/fnum value for this slot (can be different betweeen slots of one channel in 3slot mode) */
};


struct FM_ST
{
	int         clock;              /* master clock  (Hz)   */
	int         rate;               /* sampling rate (Hz)   */
	double      freqbase;           /* frequency base       */
	int         timer_prescaler;    /* timer prescaler      */
	uint8_t       address;            /* address register     */
	uint8_t       irq;                /* interrupt level      */
	uint8_t       irqmask;            /* irq mask             */
	uint8_t       status;             /* status flag          */
	uint32_t      mode;               /* mode  CSM / 3SLOT    */
	uint8_t       prescaler_sel;      /* prescaler selector   */
	uint8_t       fn_h;               /* freq latch           */
	int32_t       TA;                 /* timer a              */
	int32_t       TAC;                /* timer a counter      */
	uint8_t       TB;                 /* timer b              */
	int32_t       TBC;                /* timer b counter      */
	/* local time tables */
	int32_t       dt_tab[8][32];      /* DeTune table         */
};



/***********************************************************/
/* OPN unit                                                */
/***********************************************************/

/* OPN 3slot struct */
struct FM_3SLOT
{
	uint32_t  fc[3];          /* fnum3,blk3: calculated */
	uint8_t   fn_h;           /* freq3 latch */
	uint8_t   kcode[3];       /* key code */
	uint32_t  block_fnum[3];  /* current fnum value for this slot (can be different betweeen slots of one channel in 3slot mode) */
};

/* OPN/A/B common state */
struct FM_OPN
{
	uint8_t   type;           /* chip type */
	FM_ST   ST;             /* general state */
	FM_3SLOT SL3;           /* 3 slot mode state */
	FM_CH   *P_CH;          /* pointer of CH */
	unsigned int pan[6*2];  /* fm channels output masks (0xffffffff = enable) */

	uint32_t  eg_cnt;         /* global envelope generator counter */
	uint32_t  eg_timer;       /* global envelope generator counter works at frequency = chipclock/144/3 */
	uint32_t  eg_timer_add;   /* step of eg_timer */
	uint32_t  eg_timer_overflow;/* envelope generator timer overflows every 3 samples (on real chip) */


	/* there are 2048 FNUMs that can be generated using FNUM/BLK registers
	   but LFO works with one more bit of a precision so we really need 4096 elements */
	uint32_t  fn_table[4096]; /* fnumber->increment counter */
	uint32_t fn_max;    /* maximal phase increment (used for phase overflow) */

	/* LFO */
	uint8_t   lfo_cnt;            /* current LFO phase (out of 128) */
	uint32_t  lfo_timer;          /* current LFO phase runs at LFO frequency */
	uint32_t  lfo_timer_add;      /* step of lfo_timer */
	uint32_t  lfo_timer_overflow; /* LFO timer overflows every N samples (depends on LFO frequency) */
	uint32_t  LFO_AM;             /* current LFO AM step */
	uint32_t  LFO_PM;             /* current LFO PM step */

	int32_t   m2,c1,c2;       /* Phase Modulation input for operators 2,3,4 */
	int32_t   mem;            /* one sample delay memory */
	int32_t   out_fm[8];      /* outputs of working channels */

};





class YM2612 {
	private:
		uint8_t REGS[512]; /* registers            */
		FM_OPN OPN;		   /* OPN state            */
		FM_CH CH[6];	   /* channel state        */
		uint8_t addr_A1;   /* address line A1      */

		/* dac output (YM2612) */
		int dacen;
		int32_t dacout;

	public:
		int16_t MOL;  // master output left
		int16_t MOR;  // master output right
		YM2612();
		void reset();
		void step();
		void write(uint8_t a,uint8_t v);
		void setTl(uint8_t channel, uint8_t slot, uint8_t value);
};








#endif /* MAME_SOUND_FM_H */
