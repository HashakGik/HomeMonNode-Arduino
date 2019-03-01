#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "AnalogSignal.h"
#include "DigitalSignal.h"
#include "TemperatureSignal.h"
#include "HumiditySignal.h"
#include "SonarSignal.h"
#include "Actuator.h"
#include "Signal.h"
#include "Radio.h"

// Attaches a new signal to the internal data structures.
bool attach(Signal *s);

// Sets a trigger condition on an actuator between two signals.
void trigger(int id, comparator_t comparator, char(&a)[16], char(&b)[16]);

// Sets a trigger condition on an actuator between a signal and a constant.
void trigger(int id, comparator_t comparator, char(&a)[16], int b);

// Removes a trigger condition.
void untrigger(int id);

// Checks the serial interface for configuration commands.
void check_serial();

// Updates the signals and the actuators, and prints new values over the serial interface.
void update();

// Prints the help strings.
void help();

// Prints the current configuration.
void print();

// Parses a trigger command.
bool parse_trigger(String str);

// Parses an attach command.
bool parse_attach(String str);

extern actuator_t act[ACTUATORS_NUMBER];
extern Signal *signals[SIGNALS_NUMBER];
extern int count;
extern int pinmask;
extern unsigned long int refresh;
extern const char sensor_pin_names[SIGNALS_NUMBER][2];

#endif
