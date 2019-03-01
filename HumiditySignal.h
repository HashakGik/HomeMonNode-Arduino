#ifndef HUMIDITYSIGNAL_H
#define HUMIDITYSIGNAL_H

#include "Signal.h"
#include "DHT11.h"

/*
	Humidity signal class. Uses a DHT11 sensor to measure humidity in percent points.
	Uses one pin.
*/
class HumiditySignal :
	public Signal
{
public:
	HumiditySignal(char(&label)[16], int pin);
	~HumiditySignal();
	int Get();

private:
	Dht11 dht;
};

#endif
