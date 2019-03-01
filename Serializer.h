#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Actuator.h"
#include "Signal.h"
#include "RemoteSignal.h"
#include "EEPROM.h"

extern actuator_t act[ACTUATORS_NUMBER];
extern Signal *signals[SIGNALS_NUMBER];
extern int count;
extern unsigned long int refresh;
extern const unsigned long int default_refresh;
extern Signal *signals[SIGNALS_NUMBER];
extern RemoteSignal *remotes[MAX_REMOTES_NUMBER];
extern int remotecount;

// Clears the EEPROM and resets the data structures.
void reset_EEPROM();

// Saves the refresh rate in the EEPROM.
void serialize_refresh();

// Saves an actuator's configuration in the EEPROM.
void serialize_act(int id);

// Saves a signal's configuration in the EEPROM.
void serialize_signal(int id);

// Retrieves the saved configuration from the EEPROM.
void deserialize();

#endif
