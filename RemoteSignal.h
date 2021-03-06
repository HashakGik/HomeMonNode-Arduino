#ifndef REMOTESIGNAL_H
#define REMOTESIGNAL_H

// Maximum number of remote signals to listen to.
#define MAX_REMOTES_NUMBER 16
#define MAX_HOPS 5

#include "Signal.h"
#include "RF24.h"

extern char radio_buf[21];
extern RF24 *radio;

/*
	Remote signal class. It stores the values received over radio from other nodes.
	Doesn't use any pin.
*/
class RemoteSignal :
	public Signal
{
public:
	RemoteSignal(char(&label)[16]);
	~RemoteSignal();
	int Get();
	void Set(int x, unsigned char hops);
	unsigned char hops;
private:
	int val;

};



#endif
