/**
  * @file enigmaiot_sensor.ino
  * @version 0.3.0
  * @date 28/08/2019
  * @author German Martin
  * @brief Sensor node based on EnigmaIoT over ESP-NOW
  *
  * Sensor reading code is mocked on this example. You can implement any other code you need for your specific need
  */

#include <Arduino.h>
#include <EnigmaIOTSensor.h>
#include <espnow_hal.h>
#include <CayenneLPP.h>

#define BLUE_LED LED_BUILTIN
  //uint8_t gateway[6] = { 0x5E, 0xCF, 0x7F, 0x80, 0x34, 0x75 };
uint8_t gateway[6] = { 0xBE, 0xDD, 0xC2, 0x24, 0x14, 0x97 };

//#define SLEEP_TIME 20000000
ADC_MODE (ADC_VCC);

void connectEventHandler () {
	Serial.println ("Connected");
}

void disconnectEventHandler () {
	Serial.println ("Disconnected");
}

void processRxData (const uint8_t* mac, const uint8_t* buffer, uint8_t length) {
	char macstr[18];
	mac2str (mac, macstr);
	Serial.println ();
	Serial.printf ("Data from %s --> %s\n", macstr, printHexBuffer (buffer, length));
	for (int i = 0; i < length; i++) {
		Serial.print ((char)buffer[i]);
	}
	Serial.println ();
	Serial.println ();
}

void setup () {

	Serial.begin (115200); Serial.println (); Serial.println ();
	//time_t start = millis ();
	
	//EnigmaIOTSensor.setLed (BLUE_LED);
	pinMode (BLUE_LED, OUTPUT);
	digitalWrite (BLUE_LED, HIGH); // Turn on LED
	EnigmaIOTSensor.onConnected (connectEventHandler);
	EnigmaIOTSensor.onDisconnected (disconnectEventHandler);
	EnigmaIOTSensor.onDataRx (processRxData);

	EnigmaIOTSensor.begin (&Espnow_hal, NULL, NULL, true, false);
	//EnigmaIOTSensor.setSleepTime (5/*SLEEP_TIME / 1000000*/);

	// Read sensor data
	//msg.addAnalogInput (0, (float)(ESP.getVcc ()) / 1000);
	//Serial.printf ("Vcc: %f\n", (float)(ESP.getVcc ()) / 1000);
	//msg.addTemperature (1, 20.34);
	//// Read sensor data

	//Serial.printf ("Trying to send: %s\n", printHexBuffer (msg.getBuffer (), msg.getSize ()));

	//if (!EnigmaIOTSensor.sendData (msg.getBuffer (), msg.getSize ())) {
	//	Serial.println ("---- Error sending data");
	//} else {
	//	Serial.println ("---- Data sent");
	//}
	//Serial.printf ("Total time: %d ms\n", millis() - start);
	//EnigmaIOTSensor.sleep ();
}

void loop () {

	EnigmaIOTSensor.handle ();

	static const time_t PERIOD = 3000;
	static const time_t FLASH_DURATION = 100;
	static time_t clock;

	clock = EnigmaIOTSensor.clock () % PERIOD;

	if (EnigmaIOTSensor.hasClockSync () && EnigmaIOTSensor.isRegistered()) {
		if (clock >= 0 && clock < FLASH_DURATION) {
			digitalWrite (BLUE_LED, LOW); // Turn on LED
		} else {
			digitalWrite (BLUE_LED, HIGH); // Turn on LED
		}
	}
	
	CayenneLPP msg (20);

	static time_t lastSensorData;
	static const time_t SENSOR_PERIOD = 10000;
	if (millis () - lastSensorData > SENSOR_PERIOD) {
		lastSensorData = millis ();
		// Read sensor data
		msg.addAnalogInput (0, (float)(ESP.getVcc ()) / 1000);
		Serial.printf ("Vcc: %f\n", (float)(ESP.getVcc ()) / 1000);
		msg.addTemperature (1, 20.34);
		//// Read sensor data
		Serial.printf ("Trying to send: %s\n", printHexBuffer (msg.getBuffer (), msg.getSize ()));

		if (!EnigmaIOTSensor.sendData (msg.getBuffer (), msg.getSize ())) {
			Serial.println ("---- Error sending data");
		} else {
			Serial.println ("---- Data sent");
		}

	}
}
