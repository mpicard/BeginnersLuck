#include "plugin.hpp"

struct Sequencer : Module
{
	enum ParamIds
	{
		STEP_1_PARAM,
		STEP_2_PARAM,
		STEP_3_PARAM,
		STEP_4_PARAM,
		STEP_5_PARAM,
		STEP_6_PARAM,
		STEP_7_PARAM,
		STEP_8_PARAM,
		NUM_PARAMS
	};

	enum InputIds
	{
		CLOCK_INPUT,
		NUM_INPUTS
	};

	enum OutputIds
	{
		SEQUENCE_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		STEP_1_LIGHT,
		STEP_2_LIGHT,
		STEP_3_LIGHT,
		STEP_4_LIGHT,
		STEP_5_LIGHT,
		STEP_6_LIGHT,
		STEP_7_LIGHT,
		STEP_8_LIGHT,
		NUM_LIGHTS
	};

	dsp::SchmittTrigger clockTrigger;

	int step = 0;

	Sequencer() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = STEP_1_PARAM; i < Sequencer::NUM_PARAMS; i++)
		{
			configParam(i, 0.0, 5.0, 1.0);
		}
	}

	void process(const ProcessArgs &args) override
	{
		if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage()))
		{
			step = (step + 1) & 7;
		}

		for (int l = 0; l < NUM_LIGHTS; l++)
		{
			lights[l].setSmoothBrightness(l == step, 5e-6f);
		}

		outputs[SEQUENCE_OUTPUT].setVoltage(params[step].getValue());
	}
};

struct SequencerWidget : ModuleWidget
{
	SequencerWidget(Sequencer *module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Sequencer.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.069, 43.656)), module, Sequencer::STEP_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.676, 43.656)), module, Sequencer::STEP_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(37.284, 43.656)), module, Sequencer::STEP_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.891, 43.656)), module, Sequencer::STEP_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.069, 66.146)), module, Sequencer::STEP_5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.676, 66.146)), module, Sequencer::STEP_6_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(37.284, 66.146)), module, Sequencer::STEP_7_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.891, 66.146)), module, Sequencer::STEP_8_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.875, 25.135)), module, Sequencer::CLOCK_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(42.333, 25.135)), module, Sequencer::SEQUENCE_OUTPUT));

		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(10.069, 52.24)), module, Sequencer::STEP_1_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(23.676, 52.24)), module, Sequencer::STEP_2_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(37.284, 52.24)), module, Sequencer::STEP_3_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(50.891, 52.24)), module, Sequencer::STEP_4_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(10.069, 74.729)), module, Sequencer::STEP_5_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(23.676, 74.729)), module, Sequencer::STEP_6_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(37.284, 74.729)), module, Sequencer::STEP_7_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(50.891, 74.729)), module, Sequencer::STEP_8_LIGHT));
	}
};

Model *modelSequencer = createModel<Sequencer, SequencerWidget>("Sequencer");
