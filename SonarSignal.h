#ifndef SONARSIGNAL_H
#define SONARSIGNAL_H

#include "Signal.h"
#include "SRF05.h"

/*
	Sonar signal class. Uses an SRF-05 sensor to measure a distance in 0.1 mm increments.
	Uses two pins.
*/
class SonarSignal :
	public Signal
{
public:
	SonarSignal(char(&label)[16], unsigned int trig, unsigned int echo);
	~SonarSignal();
	int Get();

private:
	SRF05 sonar;
};

#endif
