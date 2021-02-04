#include "plugin.hpp"

struct Random : Module
{
  enum ParamIds
  {
    HOLD_PARAM,
    DISTRIBUTION_PARAM,
    NUM_PARAMS,
  };

  enum InputIds
  {
    NUM_INPUTS,
  };

  enum OutputIds
  {
    MAIN_OUTPUT,
    NUM_OUTPUTS,
  };

  enum LightIds
  {
    NUM_LIGHTS,
  };

  enum
  {
    UNIFORM = 0,
    NORMAL = 1,
    NUM_DISTRIBUTIONS
  };

  int counter;
  float value;

  Random()
  {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(HOLD_PARAM, -2.f, 6.f, 2.f, "Hold Tempo", " BPM", 2.f, 60.f, 0.f);
    configParam(DISTRIBUTION_PARAM, 0.0, 1.0, 0.0, "Distribution");
    counter = 0;
    value = 0.f;
  }

  void onSampleRateChange() override
  {
    paramQuantities[HOLD_PARAM]->displayBase = APP->engine->getSampleRate();
    paramQuantities[HOLD_PARAM]->displayMultiplier = 1.f / APP->engine->getSampleRate();
  }

  void process(const ProcessArgs &args) override
  {
    // int hold = std::floor(params[HOLD_PARAM].getValue() * args.sampleRate);
    float tempo = std::pow(2.f, params[HOLD_PARAM].getValue());
    int hold = std::floor(args.sampleRate / tempo);
    int dist = std::round(params[DISTRIBUTION_PARAM].getValue());

    if (counter >= hold)
    {
      if (dist == UNIFORM)
        value = 10.f * random::uniform();
      else
        value = clamp(5.f * random::normal(), -10.f, 10.f);
      counter = 0;
    }
    counter++;

    outputs[MAIN_OUTPUT].setVoltage(value);
  }
};

struct RandomWidget : ModuleWidget
{
  RandomWidget(Random *module)
  {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Template.svg")));

    addParam(createParam<RoundLargeBlackKnob>(Vec(30, 70), module, Random::HOLD_PARAM));
    addParam(createParam<CKSS>(Vec(38, 160), module, Random::DISTRIBUTION_PARAM));

    addOutput(createOutput<PJ3410Port>(Vec(30, 250), module, Random::MAIN_OUTPUT));
  }
};

Model *modelRandom = createModel<Random, RandomWidget>("Random");
