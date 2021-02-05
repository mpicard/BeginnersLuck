#include "RCFilter.hpp"
#include "plugin.hpp"

#define EPSILON 1e-9

template <typename T>
struct RCDiode : RCFilter<T> {
  RCDiode(T aCoefficient) {
    this->a = aCoefficient;
    this->reset();
  }

  T charge(T vi) { return this->yn = this->yn1 = vi; }
};

struct EnvFollower : Module {
  enum ParamIds {
    TAU_PARAM,
    NUM_PARAMS,
  };

  enum InputIds {
    MAIN_INPUT,
    TAU_CV_INPUT,
    NUM_INPUTS,
  };

  enum OutputIds {
    MAIN_OUTPUT,
    NUM_OUTPUTS,
  };

  enum LightsIds {
    ENV_LIGHT,
    NUM_LIGHTS,
  };

  RCDiode<float> *rcd = new RCDiode<float>(0.999f);
  float env = 0.0;

  EnvFollower() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(TAU_PARAM, 0.0, 0.5, 0.01);
  }

  void process(const ProcessArgs &args) override {
    float tau = clamp(params[TAU_PARAM].getValue(), EPSILON, 5.0);

    rcd->setTau(tau);
    float rectified = std::abs(inputs[MAIN_INPUT].getVoltage());
    if (rectified > env)
      env = rcd->charge(rectified);
    else
      env = rcd->process(rectified);

    if (outputs[MAIN_OUTPUT].isConnected()) {
      outputs[MAIN_OUTPUT].setVoltage(env);
      lights[ENV_LIGHT].value = env;
    }
  }
};

struct EnvFollowerWidget : ModuleWidget {
  EnvFollowerWidget(EnvFollower *module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Template.svg")));

    addParam(createParam<RoundBlackKnob>(Vec(30, 110), module, EnvFollower::TAU_PARAM));

    addInput(createInput<PJ301MPort>(Vec(10, 280), module, EnvFollower::MAIN_INPUT));

    addOutput(createOutput<PJ3410Port>(Vec(55, 280), module, EnvFollower::MAIN_OUTPUT));

    addChild(createLight<SmallLight<GreenLight>>(Vec(20, 310), module, EnvFollower::ENV_LIGHT));
  }
};

Model *modelEnvFollower = createModel<EnvFollower, EnvFollowerWidget>("EnvFollower");
