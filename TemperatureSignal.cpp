#include "TemperatureSignal.h"

TemperatureSignal::TemperatureSignal(char(&label)[16], int pin) :
	Signal(label, pin, UNUSED_PIN, UNUSED_PIN, UNUSED_PIN),
	dht(sensor_pins[pin])
{
	this->type = TEMPERATURE;
}


TemperatureSignal::~TemperatureSignal()
{
}

int TemperatureSignal::Get()
{
  this->dht.read();
	return this->dht.getTemperature();
}
