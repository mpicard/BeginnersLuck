#include "plugin.hpp"

Plugin *pluginInstance;

void init(Plugin *p)
{
	pluginInstance = p;

	p->addModel(modelComparator);
	p->addModel(modelMuxDemux);
	p->addModel(modelClock);
}
