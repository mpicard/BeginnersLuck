#include "plugin.hpp"

Plugin *pluginInstance;

void init(Plugin *p) {
  pluginInstance = p;

  p->addModel(modelComparator);
  p->addModel(modelMuxDemux);
  p->addModel(modelClock);
  p->addModel(modelSequencer);
  p->addModel(modelDivider);
  p->addModel(modelRandom);
  p->addModel(modelADSR);
  p->addModel(modelEnvFollower);
}
