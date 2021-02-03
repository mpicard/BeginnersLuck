#include "plugin.hpp"

#define TRIG_TIME 1e-3f

struct Clock : Module
{
  enum ParamIds
  {
    PARAM_TEMPO,
    NUM_PARAMS,
  };

  enum InputIds
  {
    NUM_INPUTS,
  };

  enum OutputIds
  {
    OUTPUT_CLOCK,
    NUM_OUTPUTS,
  };

  enum LightIds
  {
    LIGHT_CLOCK,
    NUM_LIGHTS,
  };

  dsp::PulseGenerator pg;

  float counter, period;

  Clock()
  {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(PARAM_TEMPO, 30.0, 360.0, 120.0, "Tempo", "BPM");
    counter = period = 0.f;
  }

  void process(const ProcessArgs &args) override;
};

void Clock::process(const ProcessArgs &args)
{
  float tempo = params[PARAM_TEMPO].getValue();
  period = 60.f * args.sampleRate / tempo; // samples in Hz

  if (counter > period)
  {
    pg.trigger(TRIG_TIME);
    counter -= period; // keeping the fractional part
  }

  counter++;

  float out = pg.process(args.sampleTime);

  outputs[OUTPUT_CLOCK].setVoltage(10.f * out);
  lights[LIGHT_CLOCK].setSmoothBrightness(out, 5e-6f);
}

struct ClockWidget : ModuleWidget
{
  ClockWidget(Clock *module);
};

ClockWidget::ClockWidget(Clock *module)
{
  setModule(module);
  setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Template.svg")));
  box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

  addParam(createParam<RoundBlackKnob>(Vec(30, 70), module, Clock::PARAM_TEMPO));
  addOutput(createOutput<PJ3410Port>(Vec(30, 180), module, Clock::OUTPUT_CLOCK));
  addChild(createLight<MediumLight<GreenLight>>(Vec(66, 190), module, Clock::LIGHT_CLOCK));
}

Model *modelClock = createModel<Clock, ClockWidget>("Clock");
