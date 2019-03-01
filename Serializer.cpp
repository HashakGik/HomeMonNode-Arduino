#include "Serializer.h"
#include "Controller.h"

void reset_EEPROM()
{
	for (int i = 0; i < EEPROM.length(); i++)
		EEPROM.update(i, 0xff);

	// Reset the internal data structures as well.

	pinmask = 0;
	for (int i = 0; i < count; i++)
	{
		delete signals[i];
		signals[i] = nullptr;
	}

	for (int i = 0; i < remotecount; i++)
	{
		delete remotes[i];
		remotes[i] = nullptr;
	}

	count = 0;
	remotecount = 0;
	refresh = default_refresh;
	untrigger(0);
	untrigger(1);
}

void serialize_refresh()
{
	// The refresh rate is stored at address 0.
	EEPROM.put(0, refresh);
}

void serialize_act(int id)
{
	// Each actuator is stored in 33 bytes, starting at offset 4.
	if (act[id].enabled)
	{
		// byte 0: comparator
		switch (act[id].comparator)
		{
		case EQUAL:
			EEPROM.update(4 + 33 * id, 0x00);
			break;
		case NOT_EQUAL:
			EEPROM.update(4 + 33 * id, 0x01);
			break;
		case LESS_THAN:
			EEPROM.update(4 + 33 * id, 0x02);
			break;
		case LESS_THAN_EQUAL:
			EEPROM.update(4 + 33 * id, 0x03);
			break;
		case GREATER_THAN:
			EEPROM.update(4 + 33 * id, 0x04);
			break;
		case GREATER_THAN_EQUAL:
			EEPROM.update(4 + 33 * id, 0x05);
			break;
		}
		// bytes 1-16: signal_a
		for (int i = 0; i < 16; i++)
			EEPROM.update(4 + 33 * id + 1 + i, act[id].signal_a[i]);

		// bytes 17-32: signal_b or constant (padded with 12 0xff)
		if (act[id].constcondition)
		{
			for (int i = 0; i < 12; i++)
				EEPROM.update(4 + 33 * id + 17 + i, 0xff);
			EEPROM.put(4 + 33 * id + 17 + 12, act[id].constant);
		}
		else
			for (int i = 0; i < 16; i++)
				EEPROM.update(4 + 33 * id + 17 + i, act[id].signal_b[i]);
	}
	else
		for (int i = 0; i < 33; i++)
			EEPROM.update(4 + 33 * id + i, 0xff);
}

void serialize_signal(int id)
{
	byte b;

	// Each signal is stored in 19 bytes, starting at offset 4 + 33 * ACTUATORS_NUMBER.

	if (id < count)
	{
		// bytes 0-15: label
		EEPROM.put(4 + 33 * ACTUATORS_NUMBER + 19 * id, signals[id]->label);


		// bytes 16-18: type pin[0] pin[1] pin[2] pin[3] 1111
		switch (signals[id]->type)
		{
		case ANALOG:
			b = 0;
			break;
		case DIGITAL:
			b = 1;
			break;
		case TEMPERATURE:
			b = 2;
			break;
		case HUMIDITY:
			b = 3;
			break;
		case SONAR:
			b = 4;
			break;
			/* ... */ // ADD NEW SIGNALS HERE.
		}

		b |= (signals[id]->pins[0] << 4);
		EEPROM.update(4 + 33 * ACTUATORS_NUMBER + 19 * id + 16, b);
		b = signals[id]->pins[1] | (signals[id]->pins[2] << 4);
		EEPROM.update(4 + 33 * ACTUATORS_NUMBER + 19 * id + 17, b);
		b = signals[id]->pins[3] | (0x0f << 4);
		EEPROM.update(4 + 33 * ACTUATORS_NUMBER + 19 * id + 18, b);
	}

}

void deserialize()
{
	byte b;
	int i;
	for (i = 0, b = 0xff; i < 4; i++)
		b &= EEPROM[i];

	if (b == 0xff)
		refresh = default_refresh;
	else
		EEPROM.get(0, refresh);

	for (int j = 0; j < ACTUATORS_NUMBER; j++)
	{
		for (i = 0, b = 0xff; i < 33; i++)
			b &= EEPROM[4 + 33 * j + i];

		if (b == 0xff)
			untrigger(j);
		else
		{
			switch (EEPROM[4 + 33 * j])
			{
			case 0x00:
				act[j].comparator = EQUAL;
				break;
			case 0x01:
				act[j].comparator = NOT_EQUAL;
				break;
			case 0x02:
				act[j].comparator = LESS_THAN;
				break;
			case 0x03:
				act[j].comparator = LESS_THAN_EQUAL;
				break;
			case 0x04:
				act[j].comparator = GREATER_THAN;
				break;
			case 0x05:
				act[j].comparator = GREATER_THAN_EQUAL;
				break;
			}

			for (i = 0; i < 16; i++)
				act[j].signal_a[i] = EEPROM[4 + 33 * j + 1 + i];

			for (i = 0, b = 0xff; i < 11; i++)
				b &= EEPROM[4 + 33 * j + 17 + i];

			if (b == 0xff)
			{
				act[j].constcondition = true;
				EEPROM.get(4 + 33 * j + 17 + 12, act[j].constant);
			}
			else
			{
				act[j].constcondition = false;
				for (i = 0; i < 16; i++)
					act[j].signal_b[i] = EEPROM[4 + 33 * j + 17 + i];
			}

			act[j].enabled = true;
		}
	}

	bool brk = false;
	for (int j = 0; !brk && j < SIGNALS_NUMBER; j++)
	{
		for (i = 0, b = 0xff; i < 19; i++)
			b &= EEPROM[4 + 33 * ACTUATORS_NUMBER + 19 * j + i];

		if (b == 0xff)
			brk = true;
		else
		{
			Signal *s;
			int pins[4];
			char label[16];

			EEPROM.get(4 + 33 * ACTUATORS_NUMBER + 19 * j, label);

			pins[0] = (EEPROM[4 + 33 * ACTUATORS_NUMBER + 19 * j + 16] & 0xf0) >> 4;
			pins[1] = (EEPROM[4 + 33 * ACTUATORS_NUMBER + 19 * j + 17] & 0x0f);
			pins[2] = (EEPROM[4 + 33 * ACTUATORS_NUMBER + 19 * j + 17] & 0xf0) >> 4;
			pins[3] = (EEPROM[4 + 33 * ACTUATORS_NUMBER + 19 * j + 18] & 0x0f);

			switch (EEPROM[4 + 33 * ACTUATORS_NUMBER + 19 * j + 16] & 0x0f)
			{
			case 0:
				s = new AnalogSignal(label, pins[0]);
				break;
			case 1:
				s = new DigitalSignal(label, pins[0]);
				break;
			case 2:
				s = new TemperatureSignal(label, pins[0]);
				break;
			case 3:
				s = new HumiditySignal(label, pins[0]);
				break;
			case 4:
				s = new SonarSignal(label, pins[0], pins[1]);
				break;
				/* ... */ // ADD NEW SIGNALS HERE.
			}
			attach(s);
		}
	}
}