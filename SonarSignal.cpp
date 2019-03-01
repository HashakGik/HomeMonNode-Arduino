#include "SonarSignal.h"

SonarSignal::SonarSignal(char(&label)[16], unsigned int trig, unsigned int echo) :
	Signal(label, trig, echo, UNUSED_PIN, UNUSED_PIN),
	sonar(trig, echo)
{
	this->type = SONAR;
}

SonarSignal::~SonarSignal()
{
}

int SonarSignal::Get()
{
	return (int)(this->sonar.read() * 100);
}