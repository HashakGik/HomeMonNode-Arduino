#ifndef DIGITALSIGNAL_H
#define DIGITALSIGNAL_H

#include "Signal.h"

/*
	Digital signal class. Returns 0 if the input pin is LOW and 1 if the input pin is HIGH.
	Uses one pin.
*/
class DigitalSignal :
	public Signal
{
public:
	DigitalSignal(char(&label)[16], unsigned int pin);
	~DigitalSignal();
	int Get();

private:
	unsigned int pin;
};

#endif
