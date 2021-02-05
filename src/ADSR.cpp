#include "RCFilter.hpp"
#include "dsp/digital.hpp"
#include "plugin.hpp"

#define EPSILON 1e-9

// exponential version
struct ADSR : Module {
  enum ParamIds {
    ATK_PARAM,
    DEC_PARAM,
    SUS_PARAM,
    REL_PARAM,
    NUM_PARAMS,
  };

  enum InputIds {
    GATE_INPUT,
    NUM_INPUTS,
  };

  enum OutputIds {
    ENVELOPE_OUTPUT,
    NUM_OUTPUTS,
  };

  enum LightsIds {
    GATE_LIGHT,
    NUM_LIGHTS,
  };

  dsp::SchmittTrigger gateDetect;
  RCFilter<float> *rcf = new RCFilter<float>(0.999);
  bool isAtk, isRunning;
  float env, atkTau, decTau, relTau;

  ADSR() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(ATK_PARAM, 0.0, 5.0, 0.5, "Attack Time", " s");
    configParam(DEC_PARAM, 0.0, 5.0, 0.5, "Decay Time", " s");
    configParam(SUS_PARAM, 0.0, 1.0, 0.5, "Sustain Time", " s");
    configParam(REL_PARAM, 0.0, 5.0, 0.5, "Release Time", " s");

    isAtk = false;
    isRunning = false;
    env = 0.0;
    atkTau = decTau = relTau = 0.f;
  }

  void process(const ProcessArgs &args) override {
    float sus = params[SUS_PARAM].getValue();

    atkTau = clamp(params[ATK_PARAM].getValue(), EPSILON, 5.0);
    decTau = clamp(params[DEC_PARAM].getValue(), EPSILON, 5.0);
    relTau = clamp(params[REL_PARAM].getValue(), EPSILON, 5.0);

    bool gate = inputs[GATE_INPUT].getVoltage() >= 1.0;

    if (gateDetect.process(gate)) {
      isAtk = true;
      isRunning = true;
    }

    if (isRunning) {
      if (gate) {
        if (isAtk) {
          // ATK
          rcf->setTau(atkTau);
          env = rcf->process(1.0);
          if (env >= 1.0 - 0.001) {
            isAtk = false;
          }
        } else {
          // DEC
          rcf->setTau(decTau);
          if (env <= sus + 0.001)
            env = sus;
          else
            env = rcf->process(sus);
        }
      } else {
        // REL
        rcf->setTau(relTau);
        env = rcf->process(0.0);
        if (env <= 0.001) isRunning = false;
      }
    } else {
      env = 0.0;
    }

    if (outputs[ENVELOPE_OUTPUT].isConnected()) {
      outputs[ENVELOPE_OUTPUT].setVoltage(10.0 * env);
    }
  }
};

struct ADSRWidget : ModuleWidget {
  ADSRWidget(ADSR *module) {
    setModule(module);
    setPanel(APP->window->loadSvg(
        asset::plugin(pluginInstance, "res/Template.svg")));
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    addInput(createInput<PJ301MPort>(Vec(10, 280), module, ADSR::GATE_INPUT));

    addOutput(
        createOutput<PJ301MPort>(Vec(55, 280), module, ADSR::ENVELOPE_OUTPUT));

    addParam(createParam<RoundBlackKnob>(Vec(45, 60), module, ADSR::ATK_PARAM));
    addParam(
        createParam<RoundBlackKnob>(Vec(45, 110), module, ADSR::DEC_PARAM));
    addParam(
        createParam<RoundBlackKnob>(Vec(45, 160), module, ADSR::SUS_PARAM));
    addParam(
        createParam<RoundBlackKnob>(Vec(45, 210), module, ADSR::REL_PARAM));

    addChild(createLight<SmallLight<GreenLight>>(Vec(20, 310), module,
                                                 ADSR::GATE_LIGHT));
  }
};

Model *modelADSR = createModel<ADSR, ADSRWidget>("ADSR");
