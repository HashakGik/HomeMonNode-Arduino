#include "Controller.h"
#include "Serializer.h"

void help()
{
	Serial.println(F("help\tShows this help"));
	Serial.println(F("list\tLists the current configuration"));
	Serial.println(F("clear\tResets the configuration"));

	Serial.println(F("refresh n\tSets the refresh rate to n ms (e.g. 'refresh 10000' for 10 seconds"));
	Serial.println(F("trigger actuator if var_1 comparator var_2\tEnables the trigger condition when the comparison between var_1 and var_2 is true (e.g. 'trigger act1 if bathroom_temp <= external_temp')"));
	Serial.println(F("trigger actuator if var comparator n\tEnables the trigger condition when the comparison between var and the constant n is true (e.g. 'trigger act2 if kitchen_CO2 > 500')"));
	Serial.println(F("Available comparators are: == != < <= > >="));
	Serial.println(F("untrigger actuator\tDisables the trigger condition on actuator (e.g. 'untrigger act2')"));
	Serial.println(F("attach var to signal_type pin list\tAttaches the sensor connected to the specified pins to the variable var (e.g. 'attach kitchen_temp to temperature a3', or 'attach distance to sonar d3 a7')"));
	Serial.println(F("Available signal types are: analog (1 pin), digital (1 pin), temperature (1 pin), humidity (1 pin), sonar (2 pins)"));
}

void print()
{
	Serial.print(F("Refresh rate: "));
	Serial.print(refresh);
	Serial.println(F(" ms"));

	for (int i = 0; i < ACTUATORS_NUMBER; i++)
	{
		Serial.print(F("ACT"));
		Serial.print(i + 1);
		Serial.print(F(": "));
		if (act[i].enabled)
		{
			Serial.print(F("triggered when "));
			Serial.print(act[i].signal_a);
			switch (act[i].comparator)
			{
			case EQUAL:
				Serial.print(F(" == "));
				break;
			case NOT_EQUAL:
				Serial.print(F(" != "));
				break;
			case LESS_THAN:
				Serial.print(F(" > "));
				break;
			case LESS_THAN_EQUAL:
				Serial.print(F(" <= "));
				break;
			case GREATER_THAN:
				Serial.print(F(" > "));
				break;
			case GREATER_THAN_EQUAL:
				Serial.print(F(" >= "));
				break;
			}
			if (act[i].constcondition)
				Serial.println(act[i].constant);
			else
				Serial.println(act[i].signal_b);
		}
		else
			Serial.println(F("disabled"));
	}

	for (int i = 0; i < count; i++)
	{
		Serial.print(signals[i]->label);
		Serial.print(F(" "));

		switch (signals[i]->type)
		{
		case ANALOG:
			Serial.print(F("analog"));
			break;
		case DIGITAL:
			Serial.print(F("digital"));
			break;
		case TEMPERATURE:
			Serial.print(F("temperature"));
			break;
		case HUMIDITY:
			Serial.print(F("humidity"));
			break;
		case SONAR:
			Serial.print(F("sonar"));
			break;
		}

		for (int j = 0; j < 4; j++)
			if (signals[i]->pins[j] != UNUSED_PIN)
			{
				Serial.print(F(" "));
				Serial.print(sensor_pin_names[signals[i]->pins[j]][0]);
				Serial.print(sensor_pin_names[signals[i]->pins[j]][1]);
			}
		Serial.println();
	}
}

bool parse_trigger(String str)
{
	String tok;
	char a[16], b[16];
	double d = 0;
	int i, j, pin[4];
	comparator_t comparator;

	tok = str.substring(0, str.indexOf(" "));
	str = str.substring(str.indexOf(" ") + 1);

	if (tok.substring(0, 3) == "act")
		i = (int)(tok[3] - '1');
	else
		i = -1;

	if (i < 0 || i >= ACTUATORS_NUMBER)
		Serial.println(F("Invalid actuator"));
	else
	{
		tok = str.substring(0, str.indexOf(" "));
		str = str.substring(str.indexOf(" ") + 1);

		if (tok == "if")
		{
			if (str.indexOf(" ") != -1)
			{
				tok = str.substring(0, str.indexOf(" "));
				str = str.substring(str.indexOf(" ") + 1);

				for (int k = 0; k < 16; k++)
					a[k] = (k < tok.length()) ? tok[k] : '\0';

				tok = str.substring(0, str.indexOf(" "));
				str = str.substring(str.indexOf(" ") + 1);

				bool ok = true;
				if (tok == "==")
					comparator = EQUAL;
				else if (tok == "!=")
					comparator = NOT_EQUAL;
				else if (tok == "<")
					comparator = LESS_THAN;
				else if (tok == "<=")
					comparator = LESS_THAN_EQUAL;
				else if (tok == ">")
					comparator = GREATER_THAN;
				else if (tok == ">=")
					comparator = GREATER_THAN_EQUAL;
				else
				{
					Serial.print(F("Unknown comparator"));
					ok = false;
				}

				if (ok)
				{
					if (str != tok)
					{
						j = str.toInt();
						if (str != "0" && j == 0)
						{
							for (int k = 0; k < 16; k++)
								b[k] = (k < str.length()) ? str[k] : '\0';
							trigger(i, comparator, a, b);
						}
						else
							trigger(i, comparator, a, j);

						Serial.println(F("Trigger set"));
					}
					else
						Serial.println(F("Missing operand"));
				}
			}
			else
				Serial.println(F("Missing signal"));
		}
		else
			Serial.println(F("Invalid trigger syntax"));
	}
}

bool parse_attach(String str)
{
	String tok;
	char a[16], b[16];
	int i, j, pin[4];
	comparator_t comparator;
	bool ok;

	tok = str.substring(0, str.indexOf(" "));
	str = str.substring(str.indexOf(" ") + 1);

	for (i = 0; i < 16; i++)
		a[i] = (i < tok.length()) ? tok[i] : '\0';

	tok = str.substring(0, str.indexOf(" "));
	str = str.substring(str.indexOf(" ") + 1);

	if (tok == "to")
	{
		tok = str.substring(0, str.indexOf(" "));
		str = str.substring(str.indexOf(" ") + 1);
		if (tok == "analog")
		{
			tok = str.substring(0, str.indexOf(" "));

			if (tok.length() == 2)
			{
				ok = false;
				for (i = 0; i < SIGNALS_NUMBER && !ok; i++)
					ok = tok[0] == sensor_pin_names[i][0] && tok[1] == sensor_pin_names[i][1];

				if (ok)
				{
					if (attach(new AnalogSignal(a, i - 1)))
						Serial.println(F("Ok"));
					else
						Serial.println(F("Pins already in use"));
				}
				else
					Serial.println(F("Invalid pin"));
			}
			else
				Serial.println(F("Invalid pin"));
		}
		else if (tok == "digital")
		{
			tok = str.substring(0, str.indexOf(" "));
			if (tok.length() == 2)
			{
				ok = false;
				for (i = 0; i < SIGNALS_NUMBER && !ok; i++)
					ok = tok[0] == sensor_pin_names[i][0] && tok[1] == sensor_pin_names[i][1];

				if (ok)
				{
					if (attach(new DigitalSignal(a, i - 1)))
						Serial.println(F("Ok"));
					else
						Serial.println(F("Pins already in use"));
				}
				else
					Serial.println(F("Invalid pin"));
			}
			else
				Serial.println(F("Invalid pin"));
		}
		else if (tok == "temperature")
		{
			tok = str.substring(0, str.indexOf(" "));
			if (tok.length() == 2)
			{
				ok = false;
				for (i = 0; i < SIGNALS_NUMBER && !ok; i++)
					ok = tok[0] == sensor_pin_names[i][0] && tok[1] == sensor_pin_names[i][1];

				if (ok)
				{
					if (attach(new TemperatureSignal(a, i - 1)))
						Serial.println(F("Ok"));
					else
						Serial.println(F("Pins already in use"));
				}
				else
					Serial.println(F("Invalid pin"));
			}
			else
				Serial.println(F("Invalid pin"));
		}
		else if (tok == "humidity")
		{
			tok = str.substring(0, str.indexOf(" "));
			if (tok.length() == 2)
			{
				ok = false;
				for (i = 0; i < SIGNALS_NUMBER && !ok; i++)
					ok = tok[0] == sensor_pin_names[i][0] && tok[1] == sensor_pin_names[i][1];

				if (ok)
				{
					if (attach(new HumiditySignal(a, i - 1)))
						Serial.println(F("Ok"));
					else
						Serial.println(F("Pins already in use"));
				}
				else
					Serial.println(F("Invalid pin"));
			}
			else
				Serial.println(F("Invalid pin"));
		}
		else if (tok == "sonar")
		{
			if (str.indexOf(" " != -1))
			{
				tok = str.substring(0, str.indexOf(" "));
				str = str.substring(str.indexOf(" ") + 1);

				if (tok.length() == 2)
				{
					ok = false;
					for (i = 0; i < SIGNALS_NUMBER && !ok; i++)
						ok = tok[0] == sensor_pin_names[i][0] && tok[1] == sensor_pin_names[i][1];

					if (ok)
					{
						tok = str.substring(0, str.indexOf(" "));
						str = str.substring(str.indexOf(" ") + 1);

						if (tok.length() == 2)
						{
							ok = false;
							for (j = 0; j < SIGNALS_NUMBER && !ok; j++)
								ok = tok[0] == sensor_pin_names[j][0] && tok[1] == sensor_pin_names[j][1];

							if (ok)
							{
								if (i != j && attach(new SonarSignal(a, i - 1, j - 1)))
									Serial.println(F("Ok"));
								else
									Serial.println(F("Pins already in use"));
							}
							else
								Serial.println(F("Invalid echo pin"));
						}
						else
							Serial.println(F("Invalid echo pin"));
					}
					else
						Serial.println(F("Invalid trig pin"));
				}
				else
					Serial.println(F("Invalid trig pin"));
			}
			else
				Serial.println(F("Missing pin"));
		}
		/* ... */ // ADD NEW SIGNALS HERE.
		else
			Serial.println(F("Invalid sensor type"));
	}
	else
		Serial.println(F("Syntax error"));
}

void check_serial()
{
	String str, tok;
	double d;

	if (Serial.available())
	{
		str = Serial.readStringUntil('\n');
		str.toLowerCase();
		str.trim();

		if (str == "clear")
		{
			Serial.println(F("Erasing EEPROM..."));
			reset_EEPROM();
			Serial.println(F("Done"));
		}
		else if (str == "list")
		{
			print();
		}
		else if (str == "help")
		{
			help();
		}
		else if (str.indexOf(" ") != -1)
		{
			tok = str.substring(0, str.indexOf(" "));
			str = str.substring(str.indexOf(" ") + 1);

			if (tok == "refresh")
			{
				d = str.toDouble();
				if (d == 0)
					Serial.println(F("Invalid refresh rate"));
				else
				{
					refresh = (unsigned long int) d;
					serialize_refresh();
					Serial.print(F("Set refresh rate to: "));
					Serial.println((unsigned long int) d);
				}
			}
			else if (tok == "untrigger")
			{
				if (str.substring(0, 3) == "act" && str[3] - '1' >= 0 && str[3] - '1' < ACTUATORS_NUMBER)
				{
					untrigger((int)(str[3] - '1'));
					Serial.println(F("Ok"));
				}
				else
					Serial.println(F("Invalid actuator"));
			}
			else if (tok == "attach")
			{
				parse_attach(str);
			}
			else if (tok == "trigger")
			{
				parse_trigger(str);
			}
			else
				Serial.println(F("Unknown command"));
		}
	}
}

void update()
{
	int tmp;
	for (int i = 0; i < count; i++)
	{
		tmp = signals[i]->Get();
		Serial.print(signals[i]->label);
		Serial.print(F(" "));
		Serial.print(tmp);
		Serial.print(F("\r\n"));
		if (signals[i]->type != REMOTE)
			broadcast(signals[i]->label, tmp);
	}

	for (int i = 0; i < ACTUATORS_NUMBER; i++)
	{
		if (check_condition(act[i]))
			digitalWrite(act_pins[i], HIGH);
		else
			digitalWrite(act_pins[i], LOW);
	}
}

bool attach(Signal *s)
{
	bool ret = bitRead(pinmask, s->pins[0]) == 0 &&
		bitRead(pinmask, s->pins[1]) == 0 &&
		bitRead(pinmask, s->pins[2]) == 0 &&
		bitRead(pinmask, s->pins[3]) == 0;

	if (ret && count < SIGNALS_NUMBER)
	{
		signals[count++] = s;
		serialize_signal(count - 1);

		if (s->pins[0] != UNUSED_PIN)
			bitWrite(pinmask, s->pins[0], 1);
		if (s->pins[1] != UNUSED_PIN)
			bitWrite(pinmask, s->pins[1], 1);
		if (s->pins[2] != UNUSED_PIN)
			bitWrite(pinmask, s->pins[2], 1);
		if (s->pins[3] != UNUSED_PIN)
			bitWrite(pinmask, s->pins[3], 1);
	}
	else
	{
		ret = false;
		delete s;
	}

	return ret;
}

void trigger(int id, comparator_t comparator, char(&a)[16], char(&b)[16])
{
	if (id < ACTUATORS_NUMBER)
	{
		act[id].enabled = true;
		for (int i = 0; i < 16; i++)
		{
			act[id].signal_a[i] = a[i];
			act[id].signal_b[i] = b[i];
		}

		act[id].comparator = comparator;
		act[id].constcondition = false;

		serialize_act(id);
	}
}

void trigger(int id, comparator_t comparator, char(&a)[16], int b)
{
	if (id < ACTUATORS_NUMBER)
	{
		act[id].enabled = true;
		for (int i = 0; i < 16; i++)
			act[id].signal_a[i] = a[i];
		act[id].constant = b;
		act[id].comparator = comparator;
		act[id].constcondition = true;

		serialize_act(id);
	}
}

void untrigger(int id)
{
	if (id < ACTUATORS_NUMBER)
	{
		act[id].enabled = false;
		for (int i = 0; i < 16; i++)
		{
			act[id].signal_a[i] = '\0';
			act[id].signal_b[i] = '\0';
		}

		act[id].comparator = EQUAL;
		act[id].constcondition = true;

		serialize_act(id);
	}
}
