#include "plugin.hpp"
#include "fm.h"

#define OP_VALUE(N,O,C) (params[N ## O ## _PARAM].getValue() + inputs[N ## O ## _INPUT].getPolyVoltage(C) / 10.0f)
#define OP_INT_VAL(N,C,O,MAX) ((int)(clamp(OP_VALUE(N,O,C), 0.0f, 1.0f) * MAX))
#define OP_SET(N,C,O,MAX) {uint8_t i = OP_INT_VAL(N,C,O,MAX); if(N[O+C*4]!=i){N[O+C*4]=i; ym2612.set ## N (C,O,i);}}
#define OP_SET_ALL(N,C,MAX) OP_SET(N,C,0,MAX) OP_SET(N,C,1,MAX) OP_SET(N,C,2,MAX) OP_SET(N,C,3,MAX)

#define CH_VALUE(N,C) (params[N ## _PARAM].getValue() + inputs[N ## _INPUT].getPolyVoltage(C) / 10.0f)
#define CH_INT_VAL(N,C,MAX) ((int)(clamp(CH_VALUE(N,C), 0.0f, 1.0f) * MAX))
#define CH_SET(N,C,MAX) {uint8_t i = CH_INT_VAL(N,C,MAX); if(N[C]!=i){N[C]=i; ym2612.set ## N (C,i);}}


struct CV2612Slider : SvgSlider {
	CV2612Slider() {
		minHandlePos = app::mm2px(math::Vec( 1.0, 3.2));
		maxHandlePos = app::mm2px(math::Vec(26.0, 3.2));
		horizontal = true;
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CV2612Slider.svg")));
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CV2612SliderHandle.svg")));
	}
};



struct CV2612 : Module {
	enum ParamIds {
		AR0_PARAM,
		AR1_PARAM,
		AR2_PARAM,
		AR3_PARAM,
		D10_PARAM,
		D11_PARAM,
		D12_PARAM,
		D13_PARAM,
		SL0_PARAM,
		SL1_PARAM,
		SL2_PARAM,
		SL3_PARAM,
		AL_PARAM,
		D20_PARAM,
		D21_PARAM,
		D22_PARAM,
		D23_PARAM,
		FB_PARAM,
		RR0_PARAM,
		RR1_PARAM,
		RR2_PARAM,
		RR3_PARAM,
		ST_PARAM,
		TL0_PARAM,
		TL1_PARAM,
		TL2_PARAM,
		TL3_PARAM,
		LFO_PARAM,
		MUL0_PARAM,
		MUL1_PARAM,
		MUL2_PARAM,
		MUL3_PARAM,
		AMS_PARAM,
		DET0_PARAM,
		DET1_PARAM,
		DET2_PARAM,
		DET3_PARAM,
		FMS_PARAM,
		RS0_PARAM,
		RS1_PARAM,
		RS2_PARAM,
		RS3_PARAM,
		AM0_PARAM,
		AM1_PARAM,
		AM2_PARAM,
		AM3_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		AR0_INPUT,
		AR1_INPUT,
		AR2_INPUT,
		AR3_INPUT,
		GATE_INPUT,
		D10_INPUT,
		D11_INPUT,
		D12_INPUT,
		D13_INPUT,
		SL0_INPUT,
		SL1_INPUT,
		SL2_INPUT,
		SL3_INPUT,
		AL_INPUT,
		D20_INPUT,
		D21_INPUT,
		D22_INPUT,
		D23_INPUT,
		FB_INPUT,
		RR0_INPUT,
		RR1_INPUT,
		RR2_INPUT,
		RR3_INPUT,
		ST_INPUT,
		TL0_INPUT,
		TL1_INPUT,
		TL2_INPUT,
		TL3_INPUT,
		LFO_INPUT,
		MUL0_INPUT,
		MUL1_INPUT,
		MUL2_INPUT,
		MUL3_INPUT,
		AMS_INPUT,
		DET0_INPUT,
		DET1_INPUT,
		DET2_INPUT,
		DET3_INPUT,
		FMS_INPUT,
		RS0_INPUT,
		RS1_INPUT,
		RS2_INPUT,
		RS3_INPUT,
		AM0_INPUT,
		AM1_INPUT,
		AM2_INPUT,
		AM3_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT0_OUTPUT,
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		CH0_LIGHT,
		CH1_LIGHT,
		CH2_LIGHT,
		CH3_LIGHT,
		CH4_LIGHT,
		CH5_LIGHT,
		NUM_LIGHTS
	};

	//cache

	uint8_t LFO;
	uint8_t	AL[6];
	uint8_t FB[6];
	uint8_t ST[6];
	uint8_t AMS[6];
	uint8_t FMS[6];

	uint8_t	AR[4*6];
	uint8_t D1[4*6];
	uint8_t SL[4*6];
	uint8_t D2[4*6];
	uint8_t RR[4*6];
	uint8_t TL[4*6];
	uint8_t MUL[4*6];
	uint8_t DET[4*6];
	uint8_t RS[4*6];
	uint8_t AM[4*6];

	uint8_t gated[6];

	YM2612 ym2612;

	//improves performance by fetching at a lower rate
	dsp::ClockDivider cvDivider;
	dsp::ClockDivider lightsDivider;


	CV2612() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(AR0_PARAM, 0.f, 1.f, 0.f, "Operator 1 attack rate");
		configParam(AR1_PARAM, 0.f, 1.f, 0.f, "Operator 2 attack rate");
		configParam(AR2_PARAM, 0.f, 1.f, 0.f, "Operator 3 attack rate");
		configParam(AR3_PARAM, 0.f, 1.f, 0.f, "Operator 4 attack rate");
		configParam(D10_PARAM, 0.f, 1.f, 0.f, "Operator 1 first decay rate");
		configParam(D11_PARAM, 0.f, 1.f, 0.f, "Operator 2 first decay rate");
		configParam(D12_PARAM, 0.f, 1.f, 0.f, "Operator 3 first decay rate");
		configParam(D13_PARAM, 0.f, 1.f, 0.f, "Operator 4 first decay rate");
		configParam(SL0_PARAM, 0.f, 1.f, 0.f, "Operator 1 sustain level");
		configParam(SL1_PARAM, 0.f, 1.f, 0.f, "Operator 2 sustain level");
		configParam(SL2_PARAM, 0.f, 1.f, 0.f, "Operator 3 sustain level");
		configParam(SL3_PARAM, 0.f, 1.f, 0.f, "Operator 4 sustain level");
		configParam(AL_PARAM, 0.f, 1.f, 0.f, "Algorithm");
		configParam(D20_PARAM, 0.f, 1.f, 0.f, "Operator 1 second decay rate");
		configParam(D21_PARAM, 0.f, 1.f, 0.f, "Operator 2 second decay rate");
		configParam(D22_PARAM, 0.f, 1.f, 0.f, "Operator 3 second decay rate");
		configParam(D23_PARAM, 0.f, 1.f, 0.f, "Operator 4 second decay rate");
		configParam(FB_PARAM, 0.f, 1.f, 0.f, "Feedback");
		configParam(RR0_PARAM, 0.f, 1.f, 0.f, "Operator 1 release rate");
		configParam(RR1_PARAM, 0.f, 1.f, 0.f, "Operator 2 release rate");
		configParam(RR2_PARAM, 0.f, 1.f, 0.f, "Operator 3 release rate");
		configParam(RR3_PARAM, 0.f, 1.f, 0.f, "Operator 4 release rate");
		configParam(ST_PARAM, 0.f, 1.f, 0.f, "Stereo");
		configParam(TL0_PARAM, 0.f, 1.f, 0.f, "Operator 1 total level");
		configParam(TL1_PARAM, 0.f, 1.f, 0.f, "Operator 2 total level");
		configParam(TL2_PARAM, 0.f, 1.f, 0.f, "Operator 3 total level");
		configParam(TL3_PARAM, 0.f, 1.f, 0.f, "Operator 4 total level");
		configParam(LFO_PARAM, 0.f, 1.f, 0.f, "LFO frequency");
		configParam(MUL0_PARAM, 0.f, 1.f, 0.f, "Operator 1 multiple");
		configParam(MUL1_PARAM, 0.f, 1.f, 0.f, "Operator 2 multiple");
		configParam(MUL2_PARAM, 0.f, 1.f, 0.f, "Operator 3 multiple");
		configParam(MUL3_PARAM, 0.f, 1.f, 0.f, "Operator 4 multiple");
		configParam(AMS_PARAM, 0.f, 1.f, 0.f, "Amplitude modulation sensitivity");
		configParam(DET0_PARAM, 0.f, 1.f, 0.f, "Operator 1 detune");
		configParam(DET1_PARAM, 0.f, 1.f, 0.f, "Operator 2 detune");
		configParam(DET2_PARAM, 0.f, 1.f, 0.f, "Operator 3 detune");
		configParam(DET3_PARAM, 0.f, 1.f, 0.f, "Operator 4 detune");
		configParam(FMS_PARAM, 0.f, 1.f, 0.f, "Frequency modulation sensitivity");
		configParam(RS0_PARAM, 0.f, 1.f, 0.f, "Operator 1 rate scaling");
		configParam(RS1_PARAM, 0.f, 1.f, 0.f, "Operator 2 rate scaling");
		configParam(RS2_PARAM, 0.f, 1.f, 0.f, "Operator 3 rate scaling");
		configParam(RS3_PARAM, 0.f, 1.f, 0.f, "Operator 4 rate scaling");
		configParam(AM0_PARAM, 0.f, 1.f, 0.f, "Operator 1 amplitude modulation");
		configParam(AM1_PARAM, 0.f, 1.f, 0.f, "Operator 2 amplitude modulation");
		configParam(AM2_PARAM, 0.f, 1.f, 0.f, "Operator 3 amplitude modulation");
		configParam(AM3_PARAM, 0.f, 1.f, 0.f, "Operator 4 amplitude modulation");
		ym2612.reset();
		cvDivider.setDivision(16);
		lightsDivider.setDivision(128);
	}
	void setreg(uint8_t part, uint8_t reg, uint8_t data) ;
	void process(const ProcessArgs &args) override {

		int channels = std::min(6, inputs[PITCH_INPUT].getChannels());

		//advance one sample in the emulator
		ym2612.step();

		// Compute inputs at lower rate
		if (cvDivider.process()) {


			float lfo = params[LFO_PARAM].getValue() + inputs[LFO_INPUT].getVoltage() / 10.0f;
			lfo = clamp(lfo, 0.0f, 1.0f) * 7;
			if(LFO!= (uint8_t)lfo ){
				LFO=(uint8_t)lfo;
				ym2612.setLFO(LFO);
			}

			for (int c = 0; c < channels; c++) {

				CH_SET(ST,c,3)
				CH_SET(AL,c,7)
				CH_SET(FB,c,7)
				CH_SET(AMS,c,3)
				CH_SET(FMS,c,7)

				OP_SET_ALL(AR,c,31)
				OP_SET_ALL(D1,c,31)
				OP_SET_ALL(SL,c,15)
				OP_SET_ALL(D2,c,31)
				OP_SET_ALL(RR,c,15)
				OP_SET_ALL(TL,c,127)
				OP_SET_ALL(MUL,c,15)
				OP_SET_ALL(DET,c,7)
				OP_SET_ALL(RS,c,3)
				OP_SET_ALL(AM,c,1)

				// Compute the frequency from the pitch parameter and input
				float pitch = inputs[PITCH_INPUT].getPolyVoltage(c);
				pitch = clamp(pitch, -4.f, 4.f);
				// The default pitch is C4 = 261.6256f
				float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);
				ym2612.setFREQ(c,freq);

				//check if we have to trigger a gate
				uint8_t gate = inputs[GATE_INPUT].getPolyVoltage(c) >= 1.0f;
				if (gate & !gated[c])
					ym2612.setGATE(c,gate);
				else if (!gate & gated[c])
					ym2612.setGATE(c,gate);

				gated[c] = gate;

			}

		}

		//which channels are active?
		if (lightsDivider.process()) {
			for (int c = 0; c < 6; c++) {
				lights[c].setBrightness(channels>c);
			}
		}


		// Don't forget to set the number of output channels
		outputs[OUT0_OUTPUT].setChannels(1);
		outputs[OUT1_OUTPUT].setChannels(1);

		//8192
		//8168

		outputs[OUT0_OUTPUT].setVoltage( ((float)ym2612.MOL) * 0.00010172526f );
		outputs[OUT1_OUTPUT].setVoltage( ((float)ym2612.MOR) * 0.00010172526f );

	}

};



struct CV2612Widget : ModuleWidget {
	CV2612Widget(CV2612 *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CV2612.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.337, 21.591)), module, CV2612::AR0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.657, 21.591)), module, CV2612::AR1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.977, 21.591)), module, CV2612::AR2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.297, 21.591)), module, CV2612::AR3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.337, 31.751)), module, CV2612::D10_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.657, 31.751)), module, CV2612::D11_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.977, 31.751)), module, CV2612::D12_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.297, 31.751)), module, CV2612::D13_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.337, 41.911)), module, CV2612::SL0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.657, 41.911)), module, CV2612::SL1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.977, 41.911)), module, CV2612::SL2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.297, 41.911)), module, CV2612::SL3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(26.882, 43.921)), module, CV2612::AL_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.337, 52.071)), module, CV2612::D20_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.657, 52.071)), module, CV2612::D21_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.977, 52.071)), module, CV2612::D22_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.297, 52.071)), module, CV2612::D23_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(26.882, 54.081)), module, CV2612::FB_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.337, 62.231)), module, CV2612::RR0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.657, 62.231)), module, CV2612::RR1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.977, 62.231)), module, CV2612::RR2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.297, 62.231)), module, CV2612::RR3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(26.882, 64.241)), module, CV2612::ST_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.337, 72.391)), module, CV2612::TL0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.657, 72.391)), module, CV2612::TL1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.977, 72.391)), module, CV2612::TL2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.297, 72.391)), module, CV2612::TL3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(26.882, 74.401)), module, CV2612::LFO_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.208, 83.503)), module, CV2612::MUL0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.528, 83.503)), module, CV2612::MUL1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.848, 83.503)), module, CV2612::MUL2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.168, 83.503)), module, CV2612::MUL3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(26.882, 84.561)), module, CV2612::AMS_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.208, 93.663)), module, CV2612::DET0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.528, 93.663)), module, CV2612::DET1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.848, 93.663)), module, CV2612::DET2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.168, 93.663)), module, CV2612::DET3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(26.882, 94.721)), module, CV2612::FMS_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.208, 103.823)), module, CV2612::RS0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.528, 103.823)), module, CV2612::RS1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.848, 103.823)), module, CV2612::RS2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.168, 103.823)), module, CV2612::RS3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.208, 113.983)), module, CV2612::AM0_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(81.528, 113.983)), module, CV2612::AM1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(101.848, 113.983)), module, CV2612::AM2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(122.168, 113.983)), module, CV2612::AM3_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 17.78)), module, CV2612::PITCH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4, 17.78)), module, CV2612::GATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.177, 21.591)), module, CV2612::AR0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.497, 21.591)), module, CV2612::AR1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.817, 21.591)), module, CV2612::AR2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.137, 21.591)), module, CV2612::AR3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.177, 31.751)), module, CV2612::D10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.497, 31.751)), module, CV2612::D11_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.817, 31.751)), module, CV2612::D12_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.137, 31.751)), module, CV2612::D13_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.177, 41.911)), module, CV2612::SL0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.497, 41.911)), module, CV2612::SL1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.817, 41.911)), module, CV2612::SL2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.137, 41.911)), module, CV2612::SL3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 43.921)), module, CV2612::AL_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.177, 52.071)), module, CV2612::D20_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.497, 52.071)), module, CV2612::D21_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.817, 52.071)), module, CV2612::D22_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.137, 52.071)), module, CV2612::D23_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 54.081)), module, CV2612::FB_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.177, 62.231)), module, CV2612::RR0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.497, 62.231)), module, CV2612::RR1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.817, 62.231)), module, CV2612::RR2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.137, 62.231)), module, CV2612::RR3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 64.241)), module, CV2612::ST_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.177, 72.391)), module, CV2612::TL0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.497, 72.391)), module, CV2612::TL1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.817, 72.391)), module, CV2612::TL2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.137, 72.391)), module, CV2612::TL3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 74.401)), module, CV2612::LFO_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.048, 83.503)), module, CV2612::MUL0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.368, 83.503)), module, CV2612::MUL1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.688, 83.503)), module, CV2612::MUL2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.008, 83.503)), module, CV2612::MUL3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 84.561)), module, CV2612::AMS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.048, 93.663)), module, CV2612::DET0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.368, 93.663)), module, CV2612::DET1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.688, 93.663)), module, CV2612::DET2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.008, 93.663)), module, CV2612::DET3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 94.721)), module, CV2612::FMS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.048, 103.823)), module, CV2612::RS0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.368, 103.823)), module, CV2612::RS1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.688, 103.823)), module, CV2612::RS2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.008, 103.823)), module, CV2612::RS3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.048, 113.983)), module, CV2612::AM0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.368, 113.983)), module, CV2612::AM1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.688, 113.983)), module, CV2612::AM2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(112.008, 113.983)), module, CV2612::AM3_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.182, 112.501)), module, CV2612::OUT0_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.342, 112.501)), module, CV2612::OUT1_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.08, 30.162)), module, CV2612::CH0_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16, 30.162)), module, CV2612::CH1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 30.162)), module, CV2612::CH2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.32, 30.162)), module, CV2612::CH3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.4, 30.162)), module, CV2612::CH4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 30.162)), module, CV2612::CH5_LIGHT));
		// mm2px(Vec(35.255, 15.08))
		addChild(createWidget<Widget>(mm2px(Vec(83.973, 2.62))));
	}
};


Model *modelCV2612 = createModel<CV2612, CV2612Widget>("TercerBrazo-CV2612");
