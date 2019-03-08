#include "SRF05.h"
#include "Arduino.h"

SRF05::SRF05(int trigger_pin, int echo_pin)
{
	this->trigger_pin = trigger_pin;
	this->echo_pin = echo_pin;
	pinMode(this->trigger_pin, OUTPUT);
	pinMode(this->echo_pin, INPUT);
}

int SRF05::read()
{
	long ret;

	noInterrupts();
	// Send a 10 us trigger pulse.
	digitalWrite(this->trigger_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(this->trigger_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(this->trigger_pin, LOW);

	// Convert the received echo pulse in cm.
	ret = pulseIn(this->echo_pin, HIGH);
	interrupts();

	return (int) (ret / 58L);
}
