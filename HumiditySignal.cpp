#include "HumiditySignal.h"

HumiditySignal::HumiditySignal(char(&label)[16], int pin) :
	Signal(label, pin, UNUSED_PIN, UNUSED_PIN, UNUSED_PIN),
	dht(sensor_pins[pin])
{
	this->type = HUMIDITY;
}

HumiditySignal::~HumiditySignal()
{
}

int HumiditySignal::Get()
{
	return this->dht.getHumidity();
}