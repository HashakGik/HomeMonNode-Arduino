#ifndef SIGNAL_H
#define SIGNAL_H

#define SIGNALS_NUMBER 11
#define UNUSED_PIN 15

#include <Arduino.h>

// The available sensor types.
typedef enum
{
	ANALOG,
	DIGITAL,
	TEMPERATURE,
	HUMIDITY,
	SONAR,
	REMOTE
	/* ... */ // ADD NEW SIGNALS HERE.
} signals_t;

// The available sensor pins.
const int sensor_pins[SIGNALS_NUMBER] = { A0, A1, A2, A3, A4, A5, A6, A7, 3, 4, 5 };

// Mnemonic names for the sensor pins.
const char sensor_pin_names[SIGNALS_NUMBER][2] = { "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "d3", "d4", "d5" };

/*
	Signal base class. A signal has a 16 characters label and can have up to four pins.
	The derived classes implement how the measurement of the value which will be attached to the label is done.
*/
class Signal
{
public:
	Signal(char(&label)[16], unsigned int pin0, unsigned int pin1, unsigned int pin2, unsigned int pin3);
	~Signal();
	virtual int Get() = 0;
	char label[16];
	signals_t type;
	unsigned int pins[4];
};

#endif
