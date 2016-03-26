#include <user_config.h>
#include <SmingCore/SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "network-ssid" // Put you SSID and Password here
	#define WIFI_PWD "network-pass"
#endif

#define EVENT "checkin"
#define MAKER_KEY "xxxxxxxxxxxxxxxxxxxxxx"

Timer procTimer;
uint8 bt_pin = 4;
uint8 bt_status = 0;
HttpClient ifttt;

void onDataSent(HttpClient& client, bool successful)
{
	if (successful)
		Serial.println("Success sent");
	else
		Serial.println("Failed");

	String response = client.getResponseString();
	Serial.println("Server response: '" + response + "'");
	if (response.length() > 0)
	{
		int intVal = response.toInt();

		if (intVal == 0)
			Serial.println("No response, may be we need to wait a little?");
	}
}

void sendData()
{
	if (ifttt.isProcessing()) return;

	if((digitalRead(bt_pin)) && (bt_status == 0)) {
		bt_status = 1;
		ifttt.downloadString("http://maker.ifttt.com/trigger/"+ EVENT +"/with/key/" + MAKER_KEY, onDataSent);
	}
}

// Will be called when WiFi station was connected to AP
void connectOk()
{
	Serial.println("I'm CONNECTED");

	// Start loop
	procTimer.initializeMs(25, sendData).start(); // every 25ms
}

// Will be called when WiFi station timeout was reached
void connectFail()
{
	Serial.println("I'm NOT CONNECTED. Need help :(");

	// Start soft access point
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config("CONFIG ME PLEEEEASE...", "", AUTH_OPEN);

	// .. some you code for configuration ..
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files

	pinMode(bt_pin, INPUT);

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false); // Disable debug output to serial

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	WifiStation.waitConnection(connectOk, 20, connectFail);
}
