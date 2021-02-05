#include "plugin.hpp"

struct MuxDemux : Module {
  enum ParamIds {
    PARAM_M,
    PARAM_D,
    NUM_PARAMS,
  };

  enum InputIds {
    INPUT_M1,
    INPUT_M2,
    INPUT_M3,
    INPUT_M4,
    INPUT_MAIN,
    NUM_INPUTS,
    NUM_MUX_INPUTS = INPUT_M4,
  };

  enum OutputIds {
    OUTPUT_D1,
    OUTPUT_D2,
    OUTPUT_D3,
    OUTPUT_D4,
    OUTPUT_MAIN,
    NUM_OUTPUTS,
    NUM_DEMUX_OUTPUTS = OUTPUT_D4,
  };

  enum LightIds {
    LIGHT_M1,
    LIGHT_M2,
    LIGHT_M3,
    LIGHT_M4,
    LIGHT_D1,
    LIGHT_D2,
    LIGHT_D3,
    LIGHT_D4,
    NUM_LIGHTS,
  };

  unsigned int selMux, selDemux;

  MuxDemux() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(PARAM_M, 0.0, 3.0, 0.0, "Mux Selector");
    configParam(PARAM_D, 0.0, 3.0, 0.0, "Demux Selector");
    selMux = selDemux = 0;
  }

  void process(const ProcessArgs& args) override {
    // Mux
    lights[selMux].setBrightness(0.f);
    selMux =
        (unsigned int)clamp((int)params[PARAM_M].getValue(), 0, NUM_MUX_INPUTS);
    lights[selMux].setBrightness(1.f);

    if (outputs[OUTPUT_MAIN].isConnected() && outputs[selMux].isConnected()) {
      outputs[OUTPUT_MAIN].setVoltage(inputs[selMux].getVoltage());
    }

    // Demux
    lights[selDemux + NUM_MUX_INPUTS + 1].setBrightness(0.f);
    selDemux = (unsigned int)clamp((int)params[PARAM_D].getValue(), 0,
                                   NUM_DEMUX_OUTPUTS);
    lights[selDemux + NUM_MUX_INPUTS + 1].setBrightness(1.f);

    if (inputs[INPUT_MAIN].isConnected() && outputs[selDemux].isConnected()) {
      outputs[selDemux].setVoltage(inputs[INPUT_MAIN].getVoltage());
    }
  }
};

struct MuxDemuxWidget : ModuleWidget {
#define DEMUX_Y 190

  MuxDemuxWidget(MuxDemux* module) {
    setModule(module);
    setPanel(APP->window->loadSvg(
        asset::plugin(pluginInstance, "res/Template.svg")));

    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    for (int i = MuxDemux::INPUT_M1; i < MuxDemux::INPUT_M4; i++) {
      addInput(createInput<PJ301MPort>(Vec(10, 80 + i * 30), module, i));
      addChild(
          createLight<TinyLight<GreenLight>>(Vec(40, 90 + i * 30), module, i));
    }

    addOutput(
        createOutput<PJ3410Port>(Vec(50, 100), module, MuxDemux::OUTPUT_MAIN));

    addParam(createParam<RoundBlackSnapKnob>(Vec(50, 60), module,
                                             MuxDemux::PARAM_M));

    addInput(createInput<PJ301MPort>(Vec(10, 100 + DEMUX_Y), module,
                                     MuxDemux::INPUT_MAIN));

    for (int i = MuxDemux::OUTPUT_D1; i < MuxDemux::OUTPUT_D4; i++) {
      addOutput(
          createOutput<PJ3410Port>(Vec(50, i * 30 + 50 + DEMUX_Y), module, i));
      addChild(createLight<TinyLight<GreenLight>>(
          Vec(44, i * 30 + 60 + DEMUX_Y), module, i + MuxDemux::LIGHT_D1));
    }

    addParam(createParam<RoundBlackSnapKnob>(Vec(10, 60 + DEMUX_Y), module,
                                             MuxDemux::PARAM_D));
  }
};

Model* modelMuxDemux = createModel<MuxDemux, MuxDemuxWidget>("MuxDemux");
