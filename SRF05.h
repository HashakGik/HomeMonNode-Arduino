#ifndef SRF05_H
#define SRF05_H

/*
	SRF-05 sonar class.
*/
class SRF05
{
public:
	SRF05(int trigger_pin, int echo_pin);
	int read();

private:
	int trigger_pin;
	int echo_pin;
};

#endif
