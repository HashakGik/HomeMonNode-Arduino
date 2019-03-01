#include "RemoteSignal.h"
#include "Radio.h"

RemoteSignal::RemoteSignal(char(&label)[16]) :
	Signal(label, UNUSED_PIN, UNUSED_PIN, UNUSED_PIN, UNUSED_PIN)
{
	this->val = 0;
	this->type = REMOTE;
}

RemoteSignal::~RemoteSignal()
{
}

int RemoteSignal::Get()
{
	return this->val;
}

void RemoteSignal::Set(int val)
{
	if (this->val != val)
	{
		this->val = val;
		Serial.print(this->label);
		Serial.print(" ");
		Serial.print(val);
		Serial.print("\r\n");

		radio->stopListening();
		radio->openWritingPipe(PIPE);
		radio->writeFast(radio_buf, sizeof(char) * 20);
		radio->writeFast(radio_buf, sizeof(char) * 20);
		radio->txStandBy(1000); // Long timeout for the ACK, because the commutation between transmission and reception might be slow.
		radio->openReadingPipe(0, PIPE);
		radio->startListening();
	}
}