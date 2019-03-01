#include "DigitalSignal.h"

DigitalSignal::DigitalSignal(char(&label)[16], unsigned int pin) :
	Signal(label, pin, UNUSED_PIN, UNUSED_PIN, UNUSED_PIN)
{
	this->pin = pin;
	this->type = DIGITAL;
	pinMode(pin, INPUT);
}

DigitalSignal::~DigitalSignal()
{
}

int DigitalSignal::Get()
{
	return (digitalRead(sensor_pins[this->pin]) == HIGH) ? 1 : 0;
}