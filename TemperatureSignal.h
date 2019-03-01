#ifndef TEMPERATURESIGNAL_H
#define TEMPERATURESIGNAL_H

#include "Signal.h"
#include "DHT11.h"

/*
	Temperature signal class. Uses a DHT11 sensor to measure temperature in Celsius degrees.
	Uses one pin.
*/
class TemperatureSignal :
	public Signal
{
public:
	TemperatureSignal(char(&label)[16], int pin);
	~TemperatureSignal();
	int Get();

private:
	Dht11 dht;
};

#endif