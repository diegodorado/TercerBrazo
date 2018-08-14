#include "Tutorial.hpp"


struct CV2612 : Module {
	enum ParamIds {
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
	enum InputIds {
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
	enum OutputIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	CV2612() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void CV2612::step() {
}


CV2612Widget::CV2612Widget() {
	CV2612 *module = new CV2612();
	setModule(module);
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
