#include "Controller.h"
#include "Serializer.h"
#include "Radio.h"
#include "RemoteSignal.h"

/*
	HomeMon sensor node sketch. Uses the shield from http://github.com/HashakGik/HomeMonNode-Shield-Arduino-Nano .
	This Arduino Nano sketch allows to form a peer-to-peer wireless sensor network capable of listening to up to 11 local and 32 remote signals
	and driving up to 2 actuators connected to the shield's relays.
	Each node can act as a sink for the entire network (which can be also a single stand-alone node without the radio interface),
	writing over its serial interface (and, optionally, the shield's bluetooth module) both local and remote signals.
	The sink's data can be displayed by the HomeMon application	( http://github.com/HashakGik/HomeMon-MFC-Cpp ).

	New sensors can be attached to the local signals and the actuators' trigger conditions can be set at runtime via serial commands (open a serial/bluetooth communication at
	9600 bauds, no parity, 1 stop bit, 8 data bits), type "help" for the full command list.
*/

// REMEMBER TO SET BOTH BLUETOOTH SWITCHES TO OFF WHEN UPLOADING THE SKETCH!

// Uncomment the following define if you want to clear the EEPROM when powering up the node.
// #define RESET_EEPROM

// Uncomment the following define if you want to configure the HC-05 bluetooth module. The switches must be both on in order to put the module in AT mode.
// #define BLUETOOTH_AT

// Uncomment the following define if you want to use the trimmer as analog reference.
// #define ANALOG_REF

const unsigned long int default_refresh = 1000; // Default refresh rate to be used (in ms).
const char bluetooth_name[] = "HomeMon sink"; // Name of the bluetooth module.
const char bluetooth_pin[] = "0000"; // Pin of the bluetooth module.

char radio_buf[21];
actuator_t act[ACTUATORS_NUMBER];
Signal *signals[SIGNALS_NUMBER];
RemoteSignal *remotes[MAX_REMOTES_NUMBER];
int count;
int remotecount;
int pinmask;
unsigned long int refresh;
unsigned long int ticks;
RF24 *radio = nullptr;
bool data_available = false;
bool radio_connected = false;

void setup()
{
#ifdef BLUETOOTH_AT
	Serial.begin(38400);
	Serial.print(F("AT+NAME=\""));
	Serial.print(bluetooth_name);
	Serial.print(F("\"\r\n"));
	Serial.print(F("AT+BAUD4\r\n"));
	Serial.print(F("AT+PSWD="));
	Serial.print(bluetooth_pin);
	Serial.print(F("\r\n"));
	Serial.print(F("AT+INIT"));
	Serial.print(F("\r\n"));
	delay(50);
#endif

	Serial.begin(9600);
	pinMode(10, OUTPUT); // Prevents the Arduino from being accidentally put into SPI slave mode.

	for (int i = 0; i < ACTUATORS_NUMBER; i++)
	{
		act[i].comparator = EQUAL;
		act[i].constcondition = true;
		act[i].constant = 0;
		act[i].enabled = false;
		pinMode(act_pins[i], OUTPUT);
	}

	initRadio();
	delay(50);

	ticks = 0;
	pinmask = 0;
	count = 0;
	deserialize();
	if (radio_connected)
		attachInterrupt(0, interrupt, LOW);

#ifdef ANALOG_REF
	analogReference(EXTERNAL);
#endif

#ifdef RESET_EEPROM
	reset_EEPROM();
#endif

}

void loop()
{
	if (data_available)
	{
		data_available = false;
		reroute();
	}
	else
	{
		if (ticks % 50) // Checks the serial for commands every 50 ticks.
			check_serial();

		if (ticks == 0) // Updates sensors and actuators with a rate equal to refresh.
			update();

		ticks = (ticks + 1) % refresh;
		delay(1);
	}
}

void interrupt(void)
{
	bool a, b;
	radio->whatHappened(a, b, data_available); // Clears the radio interrupt registers.
}