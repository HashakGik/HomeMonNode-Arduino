#include "Radio.h"

void initRadio()
{
	radio = new RF24(8, 9);

	radio_connected = radio->begin();
	if (radio_connected)
	{
		radio->setPALevel(RF24_PA_MAX);
		radio->setDataRate(RF24_2MBPS);
		radio->setCRCLength(RF24_CRC_16);
		radio->setAutoAck(false);
		radio->enableDynamicAck();
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

	for (int i = 0; i < MAX_REMOTES_NUMBER; i++)
		remotes[i] = nullptr;
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
			radio->read(radio_buf, sizeof(char) * 21);

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
			for (i = 0, found = false; !found && i < MAX_REMOTES_NUMBER; i++)
			{
				if (remotes[i] != nullptr)
				{
					found = true;
					for (int j = 0; j < 16; j++)
						found &= remotes[i]->label[j] == tmp[j];
				}
			}
			if (found)
				remotes[i - 1]->Set(val, radio_buf[20]);
			else
			{
				if (remotes[remotecount] == nullptr)
					remotes[remotecount] = new RemoteSignal(tmp);
				else
				{
					for (int i = 0; i < 16; i++)
						remotes[remotecount]->label[i] = tmp[i];

					remotes[remotecount]->hops = 0xff;
				}

				remotes[remotecount]->Set(val, radio_buf[20]);

				remotecount = (remotecount + 1) % MAX_REMOTES_NUMBER; // Remotes is a circular buffer.
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

		radio_buf[20] = MAX_HOPS;

		radio->stopListening();
		radio->openWritingPipe(PIPE);
		radio->writeFast(radio_buf, sizeof(char) * 21, true);
		radio->txStandBy(200);
		radio->openReadingPipe(0, PIPE);
		radio->startListening();
	}
}
