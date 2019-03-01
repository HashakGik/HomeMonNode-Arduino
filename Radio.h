#ifndef RADIO_H
#define RADIO_H

#include <SPI.h>

#include "RF24.h"
#include "nRF24L01.h"
#include "Signal.h"
#include "RemoteSignal.h"

// The radio pipe has the answer to life, the universe and everything.
#define PIPE 0x4242424242LL

extern char radio_buf[20];
extern Signal *signals[SIGNALS_NUMBER];
extern RemoteSignal *remotes[MAX_REMOTES_NUMBER];
extern int remotecount;
extern int count;
extern RF24 *radio;
extern bool radio_connected;

// Sets up the radio interface.
void initRadio();

// Reroutes with a flooding algorithm the received remote signals.
void reroute();

// Broadcasts a packet.
void broadcast(char *str, int val);

#endif
