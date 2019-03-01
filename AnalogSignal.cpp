#include "AnalogSignal.h"

AnalogSignal::AnalogSignal(char(&label)[16], unsigned int pin) :
	Signal(label, pin, UNUSED_PIN, UNUSED_PIN, UNUSED_PIN)
{
	this->pin = pin;
	pinMode(pin, INPUT);
	this->type = ANALOG;
}

AnalogSignal::~AnalogSignal()
{
}

int AnalogSignal::Get()
{
	return analogRead(sensor_pins[this->pin]);
}