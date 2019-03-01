#include "Radio.h"

void initRadio()
{
	radio = new RF24(8, 9);

	radio_connected = radio->begin();
	if (radio_connected)
	{
		radio->setPALevel(RF24_PA_HIGH);
		radio->setAutoAck(true);
		radio->enableDynamicPayloads();

		radio->maskIRQ(true, true, false); // Mask all the interrupt except the one raised when a packet is received.

		radio->openWritingPipe(PIPE);
		radio->openReadingPipe(0, PIPE);
		radio->startListening();
	}
	else
	{
		delete radio;
		radio = nullptr;
	}

	remotecount = 0;
}

void reroute()
{
	int i, val;
	char tmp[16];
	bool found;

	if (radio_connected)
	{
		if (radio->available())
			radio->read(radio_buf, sizeof(char) * 20);

		for (i = 0; i < 16; i++)
			tmp[i] = radio_buf[i];

		val = 0;
		for (i = 0; i < 8; i++)
		{
			val |= bitRead(radio_buf[16], i) << i;
			val |= bitRead(radio_buf[17], i) << (i + 8);
			val |= bitRead(radio_buf[18], i) << (i + 16);
			val |= bitRead(radio_buf[19], i) << (i + 24);
		}

		found = false;
		for (i = 0; !found && i < count; i++)
		{
			found = true;
			for (int j = 0; j < 16; j++)
				found &= signals[i]->label[j] == tmp[j];
		}
		if (!found) // Received a signal other than the local ones.
		{
			for (i = 0, found = false; !found && i < remotecount; i++)
			{
				found = true;
				for (int j = 0; j < 16; j++)
					found &= remotes[i]->label[j] == tmp[j];
			}
			if (found)
				remotes[i - 1]->Set(val);
			else
			{
				remotes[remotecount++] = new RemoteSignal(tmp);
				remotes[remotecount - 1]->Set(val);
			}
		}
	}
}

void broadcast(char *str, int val)
{
	int bucket, i, part;

	if (radio_connected)
	{
		for (i = 0; i < 16; i++)
			radio_buf[i] = str[i];

		radio_buf[16] = radio_buf[17] = radio_buf[18] = radio_buf[19] = 0;
		for (int i = 0; i < 8; i++)
		{
			radio_buf[16] |= bitRead(val, i) << i;
			radio_buf[17] |= bitRead(val, i + 8) << i;
			radio_buf[18] |= bitRead(val, i + 16) << i;
			radio_buf[19] |= bitRead(val, i + 24) << i;
		}

		radio->stopListening();
		radio->openWritingPipe(PIPE);
		radio->writeFast(radio_buf, sizeof(char) * 20);
		radio->txStandBy(1000);
		radio->openReadingPipe(0, PIPE);
		radio->startListening();
	}
}