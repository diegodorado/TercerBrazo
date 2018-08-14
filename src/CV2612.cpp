#include "TercerBrazo.hpp"

#include "fm.h"
#include <iostream>

struct CV2612 : Module {
	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		GATE_INPUT,
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;
	YM2612 ym2612;

	CV2612() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		ym2612.reset();
		initDfts();
	}
	void onReset() override;
	void onRandomize() override;
	void step() override;
	void setreg(uint8_t part, uint8_t reg, uint8_t data) ;
	void initDfts();


	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void CV2612::onReset() {
}
void CV2612::onRandomize() {
}

void CV2612::setreg(uint8_t part, uint8_t reg, uint8_t data) {
	ym2612.write((part * 2), reg);
	ym2612.write((part * 2 +1), data);
}

void CV2612::initDfts(){

	/* YM2612 Test code */
	setreg(0, 0x22, 0x00); // LFO off
	setreg(0, 0x27, 0x00); // Note off (channel 0)
	setreg(0, 0x28, 0x01); // Note off (channel 1)
	setreg(0, 0x28, 0x02); // Note off (channel 2)
	setreg(0, 0x28, 0x04); // Note off (channel 3)
	setreg(0, 0x28, 0x05); // Note off (channel 4)
	setreg(0, 0x28, 0x06); // Note off (channel 5)
	setreg(0, 0x2B, 0x00); // DAC off
	setreg(0, 0x30, 0x71); //
	setreg(0, 0x34, 0x0D); //
	setreg(0, 0x38, 0x33); //
	setreg(0, 0x3C, 0x01); // DT1/MUL
	setreg(0, 0x40, 0x23); //
	setreg(0, 0x44, 0x2D); //
	setreg(0, 0x48, 0x26); //
	setreg(0, 0x4C, 0x00); // Total level
	setreg(0, 0x50, 0x5F); //
	setreg(0, 0x54, 0x99); //
	setreg(0, 0x58, 0x5F); //
	setreg(0, 0x5C, 0x94); // RS/AR
	setreg(0, 0x60, 0x05); //
	setreg(0, 0x64, 0x05); //
	setreg(0, 0x68, 0x05); //
	setreg(0, 0x6C, 0x07); // AM/D1R
	setreg(0, 0x70, 0x02); //
	setreg(0, 0x74, 0x02); //
	setreg(0, 0x78, 0x02); //
	setreg(0, 0x7C, 0x02); // D2R
	setreg(0, 0x80, 0x11); //
	setreg(0, 0x84, 0x11); //
	setreg(0, 0x88, 0x11); //
	setreg(0, 0x8C, 0xA6); // D1L/RR
	setreg(0, 0x90, 0x00); //
	setreg(0, 0x94, 0x00); //
	setreg(0, 0x98, 0x00); //
	setreg(0, 0x9C, 0x00); // Proprietary
	setreg(0, 0xB0, 0x32); // Feedback/algorithm
	setreg(0, 0xB4, 0xC0); // Both speakers on
	setreg(0, 0x28, 0x00); // Key off
	setreg(0, 0xA4, 0x22);	//
	setreg(0, 0xA0, 0x69); // Set frequency

}

int keyon=0;
int m_step=0;
bool was_gated = false;
int last_freq=0;

void CV2612::step() {

	//advance one sample in the emulator
	ym2612.step();

	float deltaTime = engineGetSampleTime();

	// Gate and trigger
	bool gated = inputs[GATE_INPUT].value >= 1.0f;

	if (gated & !was_gated)
	{
		setreg(0, 0x28, 0x00);
	}
	else if (!gated & was_gated)
	{
		setreg(0, 0x28, 0xF0);
	}
	was_gated = gated;
	lights[BLINK_LIGHT].value = gated;

	// Compute the frequency from the pitch parameter and input
	float pitch = params[PITCH_PARAM].value;
	pitch += inputs[PITCH_INPUT].value;
	pitch = clamp(pitch, -4.0f, 4.0f);

	int freq = (int)(pitch*512);

	if (freq != last_freq)
	{
		last_freq = freq;
		setreg(0, 0xA4, freq);
		setreg(0, 0xA0, 0x69);
	}

	float val = ((float)ym2612.MOL) / 8168 * 5.0f;
	outputs[SINE_OUTPUT].value = val;
}


struct CV2612Widget : ModuleWidget {
	CV2612Widget(CV2612 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/CV2612.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, CV2612::PITCH_PARAM, -3.0, 3.0, 0.0));

		addInput(Port::create<PJ301MPort>(Vec(9, 320), Port::INPUT, module, CV2612::GATE_INPUT));

		addInput(Port::create<PJ301MPort>(Vec(9, 186), Port::INPUT, module, CV2612::PITCH_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(9, 275), Port::OUTPUT, module, CV2612::SINE_OUTPUT));

		addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, CV2612::BLINK_LIGHT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.

Model *modelCV2612 = Model::create<CV2612, CV2612Widget>("TercerBrazo", "CV2612", "CV-2612", OSCILLATOR_TAG);
