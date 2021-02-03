#include "plugin.hpp"

struct Comparator : Module
{
  enum ParamIds
  {
    NUM_PARAMS,
  };

  enum InputIds
  {
    INPUT_A1,
    INPUT_B1,
    INPUT_A2,
    INPUT_B2,
    NUM_INPUTS,
  };

  enum OutputIds
  {
    OUTPUT_1,
    OUTPUT_2,
    NUM_OUTPUTS,
  };

  enum LightIds
  {
    LIGHT_1,
    LIGHT_2,
    NUM_LIGHTS,
  };

  Comparator()
  {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
  }

  void process(const ProcessArgs &args) override;
};

void Comparator::process(const ProcessArgs &args)
{
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    int input_index = i * 2;

    if (inputs[input_index].isConnected() && inputs[input_index + 1].isConnected())
    {
      float out = inputs[input_index].getVoltage() >= inputs[input_index + 1].getVoltage();
      outputs[i].setVoltage(10.f * out);
      lights[i].setBrightness(out);
    }
  }
}

struct ComparatorWidget : ModuleWidget
{
  ComparatorWidget(Comparator *module)
  {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Comparator.svg")));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.077, 28.048)), module, Comparator::INPUT_A1));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.077, 38.631)), module, Comparator::INPUT_B1));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.783, 84.252)), module, Comparator::INPUT_A2));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.783, 95.541)), module, Comparator::INPUT_B2));

    addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(20.783, 50.146)), module, Comparator::OUTPUT_1));
    addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(20.783, 106.829)), module, Comparator::OUTPUT_2));

    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(27.5, 50.448)), module, Comparator::LIGHT_1));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(27.5, 107.069)), module, Comparator::LIGHT_1));
  }
};

Model *modelComparator = createModel<Comparator, ComparatorWidget>("Comparator");
