#include "Signal.h"

Signal::Signal(char(&label)[16], unsigned int pin0, unsigned int pin1, unsigned int pin2, unsigned int pin3) :
	type(type)
{
	for (int i = 0; i < 20; i++)
		this->label[i] = label[i];

	pins[0] = pin0;
	pins[1] = pin1;
	pins[2] = pin2;
	pins[3] = pin3;
}

Signal::~Signal()
{
}