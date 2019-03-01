#ifndef ANALOGSIGNAL_H
#define ANALOGSIGNAL_H

#include "Signal.h"

/*
	Analog signal class. Returns an analog value in [0; 1023].
	Uses one pin connected to the Arduino's ADC (A0 to A7).
*/
class AnalogSignal :
	public Signal
{
public:
	AnalogSignal(char(&label)[16], unsigned int pin);
	~AnalogSignal();
	int Get();

private:
	unsigned int pin;
};

#endif
