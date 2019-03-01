#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "Signal.h"
#include "RemoteSignal.h"

// Number of actuators.
#define ACTUATORS_NUMBER 2

// Pins attached to the actuators.
const int act_pins[ACTUATORS_NUMBER] = { 6, 7 };

extern int count;
extern int remotecount;
extern Signal *signals[SIGNALS_NUMBER];
extern RemoteSignal *remotes[MAX_REMOTES_NUMBER];

// Comparator enumeration.
typedef enum
{
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_THAN_EQUAL,
	GREATER_THAN,
	GREATER_THAN_EQUAL
} comparator_t;

// Actuator data structure. Contains two signal labels, a constant, a comparator, an enabled flag and a flag indicating if the comparison should be between the two signals or one signal and the constant.
typedef struct
{
	bool enabled;
	bool constcondition;
	char signal_a[16];
	char signal_b[16];
	int constant;
	comparator_t comparator;
} actuator_t;

// Checks if the actuator needs to be turned on.
bool check_condition(actuator_t act);

#endif
