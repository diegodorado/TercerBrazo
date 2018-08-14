#include "TercerBrazo.hpp"

#include "fm.h"
#include <iostream>

struct CV2612 : Module {
	enum ParamIds
	{
		OP1_AR_OFFSET_PARAM,
		OP2_AR_OFFSET_PARAM,
		OP3_AR_OFFSET_PARAM,
		OP4_AR_OFFSET_PARAM,
		OP1_TL_OFFSET_PARAM,
		OP2_TL_OFFSET_PARAM,
		OP3_TL_OFFSET_PARAM,
		OP4_TL_OFFSET_PARAM,
		OP1_D1R_OFFSET_PARAM,
		OP2_D1R_OFFSET_PARAM,
		OP3_D1R_OFFSET_PARAM,
		OP4_D1R_OFFSET_PARAM,
		OP1_T1L_OFFSET_PARAM,
		OP2_T1L_OFFSET_PARAM,
		OP3_T1L_OFFSET_PARAM,
		OP4_T1L_OFFSET_PARAM,
		OP1_D2R_OFFSET_PARAM,
		OP2_D2R_OFFSET_PARAM,
		OP3_D2R_OFFSET_PARAM,
		OP4_D2R_OFFSET_PARAM,
		OP1_RR_OFFSET_PARAM,
		OP2_RR_OFFSET_PARAM,
		OP3_RR_OFFSET_PARAM,
		OP4_RR_OFFSET_PARAM,
		NUM_PARAMS
	};
	enum InputIds
	{
		GATE_INPUT,
		OP1_AR_INPUT,
		OP2_AR_INPUT,
		OP3_AR_INPUT,
		OP4_AR_INPUT,
		CV_INPUT,
		OP1_TL_INPUT,
		OP2_TL_INPUT,
		OP3_TL_INPUT,
		OP4_TL_INPUT,
		OP1_D1R_INPUT,
		OP2_D1R_INPUT,
		OP3_D1R_INPUT,
		OP4_D1R_INPUT,
		OP1_T1L_INPUT,
		OP2_T1L_INPUT,
		OP3_T1L_INPUT,
		OP4_T1L_INPUT,
		OP1_D2R_INPUT,
		OP2_D2R_INPUT,
		OP3_D2R_INPUT,
		OP4_D2R_INPUT,
		OP1_RR_INPUT,
		OP2_RR_INPUT,
		OP3_RR_INPUT,
		OP4_RR_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

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
	//lights[BLINK_LIGHT].value = gated;

	float tl = params[OP1_TL_OFFSET_PARAM].value;
	tl = clamp(tl, 0.0f, 1.0f);
	ym2612.setTl(0,0,(int)(tl*127));

	// Compute the frequency from the pitch parameter and input
	float pitch = inputs[CV_INPUT].value;
	pitch = clamp(pitch, -4.0f, 4.0f);

	int freq = (int)(pitch*512);

	if (freq != last_freq)
	{
		last_freq = freq;
		setreg(0, 0xA4, freq);
		setreg(0, 0xA0, 0x69);
	}

	outputs[LEFT_OUTPUT].value = ((float)ym2612.MOL) / 8168 * 5.0f;
	outputs[RIGHT_OUTPUT].value = ((float)ym2612.MOR) / 8168 * 5.0f;
}

struct CV2612Widget : ModuleWidget {
	CV2612Widget(CV2612 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/CV2612.svg")));

		addChild(createScrew<ScrewSilver>(Vec(15, 0)));
		addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createScrew<ScrewSilver>(Vec(15, 365)));
		addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 365)));

		addParam(createParam<Davies1900hBlackKnob>((Vec(114.867, 41.632)), module, CV2612::OP1_AR_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(194.665, 41.632)), module, CV2612::OP2_AR_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(274.464, 41.632)), module, CV2612::OP3_AR_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(354.262, 41.632)), module, CV2612::OP4_AR_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(115.875, 85.142)), module, CV2612::OP1_TL_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(195.673, 85.142)), module, CV2612::OP2_TL_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(275.471, 85.142)), module, CV2612::OP3_TL_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(355.27, 85.142)), module, CV2612::OP4_TL_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(114.867, 129.98)), module, CV2612::OP1_D1R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(194.665, 129.98)), module, CV2612::OP2_D1R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(274.464, 129.98)), module, CV2612::OP3_D1R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(354.262, 129.98)), module, CV2612::OP4_D1R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(115.875, 177.842)), module, CV2612::OP1_T1L_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(195.673, 177.842)), module, CV2612::OP2_T1L_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(275.471, 177.842)), module, CV2612::OP3_T1L_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(355.27, 177.842)), module, CV2612::OP4_T1L_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(115.875, 222.68)), module, CV2612::OP1_D2R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(195.673, 222.68)), module, CV2612::OP2_D2R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(275.471, 222.68)), module, CV2612::OP3_D2R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(355.27, 222.68)), module, CV2612::OP4_D2R_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(115.773, 270.541)), module, CV2612::OP1_RR_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(195.572, 270.541)), module, CV2612::OP2_RR_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(275.37, 270.541)), module, CV2612::OP3_RR_OFFSET_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Davies1900hBlackKnob>((Vec(355.168, 270.541)), module, CV2612::OP4_RR_OFFSET_PARAM, 0.0, 1.0, 0.0));

		addInput(createInput<PJ301MPort>((Vec(21.949, 35.31)), module, CV2612::GATE_INPUT));
		addInput(createInput<PJ301MPort>((Vec(81.857, 53.324)), module, CV2612::OP1_AR_INPUT));
		addInput(createInput<PJ301MPort>((Vec(161.655, 53.324)), module, CV2612::OP2_AR_INPUT));
		addInput(createInput<PJ301MPort>((Vec(241.453, 53.324)), module, CV2612::OP3_AR_INPUT));
		addInput(createInput<PJ301MPort>((Vec(321.252, 53.324)), module, CV2612::OP4_AR_INPUT));
		addInput(createInput<PJ301MPort>((Vec(21.949, 75.209)), module, CV2612::CV_INPUT));
		addInput(createInput<PJ301MPort>((Vec(82.864, 96.834)), module, CV2612::OP1_TL_INPUT));
		addInput(createInput<PJ301MPort>((Vec(162.663, 96.834)), module, CV2612::OP2_TL_INPUT));
		addInput(createInput<PJ301MPort>((Vec(242.461, 96.834)), module, CV2612::OP3_TL_INPUT));
		addInput(createInput<PJ301MPort>((Vec(322.259, 96.834)), module, CV2612::OP4_TL_INPUT));
		addInput(createInput<PJ301MPort>((Vec(81.857, 141.672)), module, CV2612::OP1_D1R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(161.655, 141.672)), module, CV2612::OP2_D1R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(241.453, 141.672)), module, CV2612::OP3_D1R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(321.252, 141.672)), module, CV2612::OP4_D1R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(82.864, 189.534)), module, CV2612::OP1_T1L_INPUT));
		addInput(createInput<PJ301MPort>((Vec(162.663, 189.534)), module, CV2612::OP2_T1L_INPUT));
		addInput(createInput<PJ301MPort>((Vec(242.461, 189.534)), module, CV2612::OP3_T1L_INPUT));
		addInput(createInput<PJ301MPort>((Vec(322.259, 189.534)), module, CV2612::OP4_T1L_INPUT));
		addInput(createInput<PJ301MPort>((Vec(82.864, 234.372)), module, CV2612::OP1_D2R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(162.663, 234.372)), module, CV2612::OP2_D2R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(242.461, 234.372)), module, CV2612::OP3_D2R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(322.259, 234.372)), module, CV2612::OP4_D2R_INPUT));
		addInput(createInput<PJ301MPort>((Vec(82.763, 282.233)), module, CV2612::OP1_RR_INPUT));
		addInput(createInput<PJ301MPort>((Vec(162.561, 282.233)), module, CV2612::OP2_RR_INPUT));
		addInput(createInput<PJ301MPort>((Vec(242.36, 282.233)), module, CV2612::OP3_RR_INPUT));
		addInput(createInput<PJ301MPort>((Vec(322.158, 282.233)), module, CV2612::OP4_RR_INPUT));

		addOutput(createOutput<PJ301MPort>((Vec(333.163, 342.534)), module, CV2612::LEFT_OUTPUT));
		addOutput(createOutput<PJ301MPort>((Vec(364.654, 342.534)), module, CV2612::RIGHT_OUTPUT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.

Model *modelCV2612 = Model::create<CV2612, CV2612Widget>("TercerBrazo", "CV2612", "CV-2612", OSCILLATOR_TAG);
