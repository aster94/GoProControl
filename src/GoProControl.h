/*
GoProControl.h

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GOPRO_CONTROL_H
#define GOPRO_CONTROL_H

#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <Settings.h>

// include the correct wifi library
#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
#include <WiFi.h>
#include <WiFiUdp.h>
// todo include BLE library
#elif defined(ARDUINO_SAMD_MKR1000) // MKR1000
#define HardwareSerial Serial_
#include <WiFi101.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_MKRWIFI1010) // MKR WiFi 1010
#define HardwareSerial Serial_
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_MKRVIDOR4000) // MKR VIDOR 4000
#define HardwareSerial Serial_
#include <VidorPeripherals.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2) // UNO WiFi Rev.2
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#else // any board (like arduino UNO) without wifi + ESP01 with AT commands
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#define WiFiClient WiFiEspClient
#define WiFiUDP WiFiEspUDP
#warning "Are you using an ESP01 + AT commands? if not open an issue on github: https://github.com/aster94/GoProControl"
#endif


class GoProControl
{
  public:
	// constructors
	GoProControl(const String ssid, const String pwd, const uint8_t camera);														// for HERO3 or older
	GoProControl(const String ssid, const String pwd, const uint8_t camera, const byte mac_address[], const String board_name); // for HERO4 or newer

	// Comunication
	uint8_t begin();
	void end();
	uint8_t keepAlive();

// BLE functions are availables only on ESP32
#if defined(ARDUINO_ARCH_ESP32)
	// none of these function will work, I am adding these for a proof of concept
	// https://github.com/KonradIT/goprowifihack/blob/master/HERO5/HERO5-Commands.md#bluetooth-pairing
	uint8_t enableBLE();
	uint8_t disableBLE();
	uint8_t wifiOff();
	uint8_t wifiOn();
#endif

	// Control
	uint8_t turnOn();
	uint8_t turnOff();
	uint8_t isOn();
	uint8_t checkConnection(const uint8_t silent = false); // maybe move to private

	// Shoot
	uint8_t shoot();
	uint8_t stopShoot();

	// Settings
	uint8_t setMode(const uint8_t option);
	uint8_t setOrientation(const uint8_t option);

	// Video
	uint8_t setVideoResolution(const uint8_t option);
	uint8_t setVideoFov(const uint8_t option);
	uint8_t setFrameRate(const uint8_t option);
	uint8_t setVideoEncoding(const uint8_t option);

	// Photo
	uint8_t setPhotoResolution(const uint8_t option);
	uint8_t setTimeLapseInterval(float option);
	uint8_t setContinuousShot(const uint8_t option);

	// Others
	uint8_t localizationOn();
	uint8_t localizationOff();
	uint8_t deleteLast();
	uint8_t deleteAll();

	// Debug
	void enableDebug(HardwareSerial *debug_port, const uint32_t debug_baudrate = 115200);
	void disableDebug();
	uint8_t getStatus();
	void printStatus();

  private:
	WiFiClient _wifi_client;
	const String _host = "10.5.5.9";
	const uint16_t _wifi_port = 80;
	const uint8_t _udp_port = 9;
	HttpClient _http = HttpClient(_wifi_client, _host, _wifi_port);

	String _ssid;
	String _pwd;
	uint8_t _camera;

	WiFiUDP _udp_client;
	uint8_t _mac_address[6];
	String _board_name;

	String _url;
	uint8_t WIFI_MODE = true;
	uint8_t BLE_ENABLED = false;

	uint8_t _connected = false;
	uint64_t _last_request;

	HardwareSerial *_debug_port;
	uint8_t _debug;

	void sendWoL();
	uint8_t sendRequest(const String request);
	uint8_t sendHTTPRequest(const String request);
#if defined(ARDUINO_ARCH_ESP32)
	uint8_t sendBLERequest(const uint8_t request[]);
#endif
	uint8_t connectClient();
	uint8_t confirmPairing();
};

#endif //GOPRO_CONTROL_H
