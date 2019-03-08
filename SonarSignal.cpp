#include "SonarSignal.h"

SonarSignal::SonarSignal(char(&label)[16], unsigned int trig, unsigned int echo) :
	Signal(label, trig, echo, UNUSED_PIN, UNUSED_PIN),
	sonar(sensor_pins[trig], sensor_pins[echo])
{
	this->type = SONAR;
}

SonarSignal::~SonarSignal()
{
}

int SonarSignal::Get()
{
	return this->sonar.read();
}
