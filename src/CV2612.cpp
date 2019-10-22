#include "plugin.hpp"
#include "fm.h"


struct CV2612Knob : RoundSmallBlackKnob {
	CV2612Knob() {
		snap = true;
	}

};


struct CV2612 : Module {

	enum ParamIds {
		AL_PARAM,
		FB_PARAM,
		LFO_PARAM,
		AMS_PARAM,
		FMS_PARAM,
		ENUMS(AR_PARAMS, 4),
		ENUMS(D1_PARAMS, 4),
		ENUMS(SL_PARAMS, 4),
		ENUMS(D2_PARAMS, 4),
		ENUMS(RR_PARAMS, 4),
		ENUMS(TL_PARAMS, 4),
		ENUMS(MUL_PARAMS, 4),
		ENUMS(DET_PARAMS, 4),
		ENUMS(RS_PARAMS, 4),
		ENUMS(AM_PARAMS, 4),
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		GATE_INPUT,
		AL_INPUT,
		FB_INPUT,
		LFO_INPUT,
		AMS_INPUT,
		FMS_INPUT,
		ENUMS(AR_INPUTS, 4),
		ENUMS(D1_INPUTS, 4),
		ENUMS(SL_INPUTS, 4),
		ENUMS(D2_INPUTS, 4),
		ENUMS(RR_INPUTS, 4),
		ENUMS(TL_INPUTS, 4),
		ENUMS(MUL_INPUTS, 4),
		ENUMS(DET_INPUTS, 4),
		ENUMS(RS_INPUTS, 4),
		ENUMS(AM_INPUTS, 4),
		NUM_INPUTS
	};
	enum OutputIds {
		OUT0_OUTPUT,
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(CH_LIGHTS, 6),
		NUM_LIGHTS
	};


	uint8_t	algorithm = 7;
	uint8_t gated[6];

	YM2612 ym2612;

	//improves performance by fetching at a lower rate
	dsp::ClockDivider cvDivider;
	dsp::ClockDivider lightsDivider;


	CV2612() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		for (int i = 0; i < 4; i++) {
			configParam(  AR_PARAMS + i, 0.f, 31.f , 31.f, "Operator " + (i+1)+ std::string(" Attack Rate"));
			configParam(  D1_PARAMS + i, 0.f, 31.f , 0.f , "Operator " + (i+1)+ std::string(" 1st Decay Rate"));
			configParam(  SL_PARAMS + i, 0.f, 15.f , 0.f , "Operator " + (i+1)+ std::string(" Sustain Level"));
			configParam(  D2_PARAMS + i, 0.f, 31.f , 0.f , "Operator " + (i+1)+ std::string(" 2nd Decay Rate"));
			configParam(  RR_PARAMS + i, 0.f, 15.f , 15.f, "Operator " + (i+1)+ std::string(" Release Rate"));
			configParam(  TL_PARAMS + i, 0.f, 127.f, 0.f , "Operator " + (i+1)+ std::string(" Total Level"));
			configParam( MUL_PARAMS + i, 0.f, 15.f , 1.f , "Operator " + (i+1)+ std::string(" Multiplier"));
			configParam( DET_PARAMS + i, 0.f, 7.f  , 0.f , "Operator " + (i+1)+ std::string(" Detune"));
			configParam(  RS_PARAMS + i, 0.f, 3.f  , 0.f , "Operator " + (i+1)+ std::string(" Rate Scaling"));
			configParam(  AM_PARAMS + i, 0.f, 1.f  , 0.f , "Operator " + (i+1)+ std::string(" Amplitude Modulation"));
		}

		
		configParam(  AL_PARAM, 0.f, 7.f, 7.f, "Algorithm");
		configParam(  FB_PARAM, 0.f, 7.f, 0.f, "Feedback");
		configParam( LFO_PARAM, 0.f, 7.f, 0.f, "LFO frequency");
		configParam( AMS_PARAM, 0.f, 3.f, 0.f, "Amplitude modulation sensitivity");
		configParam( FMS_PARAM, 0.f, 7.f, 0.f, "Frequency modulation sensitivity");

		ym2612.reset();
		cvDivider.setDivision(16);
		lightsDivider.setDivision(128);

	}

	uint8_t computeValue(int channel, int param, int input, int max){
		return clamp((int)(params[param].getValue() + ((inputs[input].getPolyVoltage(channel) / 10.0f)*max)), 0, max); 
	}

	void setreg(uint8_t part, uint8_t reg, uint8_t data) ;
	void process(const ProcessArgs &args) override {

		int channels = std::min(6, inputs[PITCH_INPUT].getChannels());

		//advance one sample in the emulator
		ym2612.step();

		// Compute inputs at lower rate
		if (cvDivider.process()) {
			
			// this value is used in the algorithm widget
			algorithm = (uint8_t) params[AL_PARAM].getValue();

			ym2612.setLFO (computeValue(0, LFO_PARAM,  LFO_INPUT, 7));

			for (int c = 0; c < channels; c++) {
				ym2612.setAL  (c, computeValue(c, AL_PARAM,  AL_INPUT, 7));
				ym2612.setFB  (c, computeValue(c, FB_PARAM,  FB_INPUT, 7));
				ym2612.setAMS (c, computeValue(c,AMS_PARAM, AMS_INPUT, 3));
				ym2612.setFMS (c, computeValue(c,FMS_PARAM, FMS_INPUT, 7));

				for (int o = 0; o < 4; o++) {
					ym2612.setAR  (c, o , computeValue(c,  AR_PARAMS + o ,  AR_INPUTS + o, 31 ));
					ym2612.setD1  (c, o , computeValue(c,  D1_PARAMS + o ,  D1_INPUTS + o, 31 ));
					ym2612.setSL  (c, o , computeValue(c,  SL_PARAMS + o ,  SL_INPUTS + o, 15 ));
					ym2612.setD2  (c, o , computeValue(c,  D2_PARAMS + o ,  D2_INPUTS + o, 31 ));
					ym2612.setRR  (c, o , computeValue(c,  RR_PARAMS + o ,  RR_INPUTS + o, 15 ));
					ym2612.setTL  (c, o , computeValue(c,  TL_PARAMS + o ,  TL_INPUTS + o, 127));
					ym2612.setMUL (c, o , computeValue(c, MUL_PARAMS + o , MUL_INPUTS + o, 15 ));
					ym2612.setDET (c, o , computeValue(c, DET_PARAMS + o , DET_INPUTS + o, 7  ));
					ym2612.setRS  (c, o , computeValue(c,  RS_PARAMS + o ,  RS_INPUTS + o, 3  ));
					ym2612.setAM  (c, o , computeValue(c,  AM_PARAMS + o ,  AM_INPUTS + o, 1  ));
				}


				// Compute the frequency from the pitch parameter and input
				float pitch = inputs[PITCH_INPUT].getPolyVoltage(c);
				pitch = clamp(pitch, -4.f, 6.f);
				// The default pitch is C4 = 261.6256f
				float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);
				ym2612.setFREQ(c,freq);

				//check if we have to trigger a gate
				uint8_t gate = inputs[GATE_INPUT].getPolyVoltage(c) >= 1.0f;
				if (gate & !gated[c])
					ym2612.setGATE(c,gate);
				else if ((!gate) & gated[c])
					ym2612.setGATE(c,gate);

				gated[c] = gate;

			}


			// close gate of unsed voice
			// todo: close only if necesary
			for (int c = channels; c < 6; c++) {
				//if (gated[c]){
					ym2612.setGATE(c,0);
				//	gated[c] = 0;
				//}
			}


		}

		//which channels are active?
		if (lightsDivider.process()) {
			for (int c = 0; c < 6; c++) {
				lights[CV2612::CH_LIGHTS + c].setBrightness(channels>c);
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



struct AlgorithmDisplay : TransparentWidget {
	CV2612 *module;
	std::vector<NSVGimage*> frames;

	AlgorithmDisplay() {
		for(int i = 0;i<8;i++){
			addFrame(asset::plugin(pluginInstance, "res/alg/"+std::to_string(i)+".svg"));
		}
	}

	void addFrame(std::string  path) {
		NSVGimage* image;
		// 17.5dpi means nothing... i am being lazy to rescale
		// all my svg properly ... so i am rescaling "with dpi" =(
		image = nsvgParseFromFile(path.c_str(), "mm", 16.5);
		frames.push_back(image);
	};

	void draw(const DrawArgs &args) override {
		uint8_t algorithm = (module) ? module->algorithm : 7;

		NVGcolor backgroundColor = nvgRGB(0x38, 0x38, 0x38);
		NVGcolor borderColor = nvgRGB(0x50, 0x9e, 0xec);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 2.0);
		nvgFillColor(args.vg, backgroundColor);
		nvgFill(args.vg);
		nvgStrokeWidth(args.vg, 1.0);
		nvgStrokeColor(args.vg, borderColor);
		nvgStroke(args.vg);

		svgDraw(args.vg,frames[algorithm]);
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


		uint8_t spacing = 20;
		for (int i = 0; i < 4; i++) {
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 21.591)), module, CV2612::AR_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 31.751)), module, CV2612::D1_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 41.911)), module, CV2612::SL_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 52.071)), module, CV2612::D2_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 62.231)), module, CV2612::RR_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 72.391)), module, CV2612::TL_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 83.503)), module, CV2612::MUL_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 93.663)), module, CV2612::DET_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 103.823)), module, CV2612::RS_PARAMS + i));
			addParam(createParamCentered<CV2612Knob>(mm2px(Vec(62+spacing*i, 113.983)), module, CV2612::AM_PARAMS + i));

			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 21.591)), module, CV2612::AR_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 31.751)), module, CV2612::D1_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 41.911)), module, CV2612::SL_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 52.071)), module, CV2612::D2_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 62.231)), module, CV2612::RR_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 72.391)), module, CV2612::TL_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 83.503)), module, CV2612::MUL_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 93.663)), module, CV2612::DET_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 103.823)), module, CV2612::RS_INPUTS + i));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52+spacing*i, 113.983)), module, CV2612::AM_INPUTS + i));

		}


		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.758, 13.547)), module, CV2612::PITCH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.458, 13.547)), module, CV2612::GATE_INPUT));

		addParam(createParamCentered<CV2612Knob>(mm2px(Vec(26.882, 56.621)), module, CV2612::AL_PARAM));
		addParam(createParamCentered<CV2612Knob>(mm2px(Vec(26.882, 66.252)), module, CV2612::FB_PARAM));
		addParam(createParamCentered<CV2612Knob>(mm2px(Vec(26.882, 75.988)), module, CV2612::LFO_PARAM));
		addParam(createParamCentered<CV2612Knob>(mm2px(Vec(26.882, 85.619)), module, CV2612::AMS_PARAM));
		addParam(createParamCentered<CV2612Knob>(mm2px(Vec(26.882, 94.721)), module, CV2612::FMS_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 56.621)), module, CV2612::AL_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 66.252)), module, CV2612::FB_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 75.988)), module, CV2612::LFO_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 85.619)), module, CV2612::AMS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.722, 94.721)), module, CV2612::FMS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.182, 112.501)), module, CV2612::OUT0_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.342, 112.501)), module, CV2612::OUT1_OUTPUT));


		for (int i = 0; i < 6; i++) {
			addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(6+5*i, 23.283)), module, CV2612::CH_LIGHTS + i));
		}



		{
			AlgorithmDisplay *display = new AlgorithmDisplay();
			display->box.pos = mm2px(Vec(4.749, 29.698));
			display->box.size = mm2px(Vec(28.073, 17)); //height was 21.15
			display->module = module;
			addChild(display);
		}



	}

};


Model *modelCV2612 = createModel<CV2612, CV2612Widget>("TercerBrazo-CV2612");
