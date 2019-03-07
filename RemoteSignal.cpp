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
	this->hops = 0;
}

int RemoteSignal::Get()
{
	return this->val;
}

void RemoteSignal::Set(int val, unsigned char hops)
{
	if (this->val != val)
	{
		this->val = val;

	  Serial.print(this->label);
		Serial.print(" ");
		Serial.print(val);
		Serial.print("\r\n");
	}

	if (hops > 0)
	{
		for (int i = 0; i < 16; i++)
			radio_buf[i] = this->label[i];

		radio_buf[16] = radio_buf[17] = radio_buf[18] = radio_buf[19] = 0;
		for (int i = 0; i < 8; i++)
		{
			radio_buf[16] |= bitRead(val, i) << i;
			radio_buf[17] |= bitRead(val, i + 8) << i;
			radio_buf[18] |= bitRead(val, i + 16) << i;
			radio_buf[19] |= bitRead(val, i + 24) << i;
		}

		radio_buf[20] = hops - 1;

		radio->stopListening();
		radio->openWritingPipe(PIPE);
		radio->writeFast(radio_buf, sizeof(char) * 21, true);
		radio->writeFast(radio_buf, sizeof(char) * 21, true);
		radio->txStandBy(200); // Long timeout for the ACK, because the commutation between transmission and reception might be slow.
		radio->openReadingPipe(0, PIPE);
		radio->startListening();
	}
}
