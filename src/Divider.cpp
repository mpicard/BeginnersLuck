#include "plugin.hpp"

#define TRIG_TIME 1e-3f

struct Divider : Module {
  enum ParamIds { NUM_PARAMS };

  enum InputIds { MAIN_INPUT, RESET_INPUT, NUM_INPUTS };

  enum OutputIds {
    OUTPUT1_OUTPUT,  // hidden
    OUTPUT2_OUTPUT,
    OUTPUT4_OUTPUT,
    OUTPUT8_OUTPUT,
    OUTPUT16_OUTPUT,
    OUTPUT32_OUTPUT,
    NUM_OUTPUTS
  };

  enum LightIds {
    LIGHT1_LIGHT,  // hidden
    LIGHT2_LIGHT,
    LIGHT4_LIGHT,
    LIGHT8_LIGHT,
    LIGHT16_LIGHT,
    LIGHT32_LIGHT,
    NUM_LIGHTS
  };

  dsp::ClockDivider dividers[NUM_OUTPUTS];
  dsp::PulseGenerator pg[NUM_OUTPUTS];
  dsp::SchmittTrigger clockTrigger;
  dsp::SchmittTrigger resetTrigger;

  Divider() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      dividers[i].setDivision(2);
    }
  }

  void iterActive(int i) {
    if (i > NUM_OUTPUTS - 1) return;

    bool active = dividers[i].process();
    pg[i].trigger(TRIG_TIME);

    if (active) {
      iterActive(i + 1);
    }
  }

  void process(const ProcessArgs& args) override {
    if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        dividers[i].reset();
      }
    }

    if (clockTrigger.process(inputs[MAIN_INPUT].getVoltage())) {
      iterActive(0);  // this will run the first divider and iterate through next
    }

    for (int i = 0; i < NUM_OUTPUTS; i++) {
      float out = pg[i].process(args.sampleTime);
      outputs[i].setVoltage(10.f * out);
      lights[i].setSmoothBrightness(out, 5e-6f);
    }
  }
};

struct DividerWidget : ModuleWidget {
  DividerWidget(Divider* module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Divider.svg")));

    addInput(
        createInputCentered<PJ301MPort>(mm2px(Vec(11.812, 24.746)), module, Divider::MAIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21, 24.746)), module, Divider::RESET_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.261, 52.338)), module,
                                               Divider::OUTPUT2_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.261, 66.954)), module,
                                               Divider::OUTPUT4_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.261, 81.511)), module,
                                               Divider::OUTPUT8_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.261, 96.035)), module,
                                               Divider::OUTPUT16_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.261, 110.615)), module,
                                               Divider::OUTPUT32_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.78, 44.878)), module,
                                                        Divider::LIGHT2_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.78, 59.166)), module,
                                                        Divider::LIGHT4_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.78, 73.982)), module,
                                                        Divider::LIGHT8_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.732, 88.515)), module,
                                                        Divider::LIGHT16_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.78, 103.086)), module,
                                                        Divider::LIGHT32_LIGHT));
  }
};

Model* modelDivider = createModel<Divider, DividerWidget>("Divider");
