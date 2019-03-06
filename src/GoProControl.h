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


#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WiFiUdp.h>
#endif


class GoProControl
{
  public:
	GoProControl(WiFiClient client, String ssid, String pwd, uint8_t camera);
	uint8_t begin();
	static void sendWoL(WiFiUDP udp, byte *mac, size_t size_of_mac);
	uint8_t confirmPairing();

	// on/off
	uint8_t turnOn();
	uint8_t turnOff();

	// Shoot
	uint8_t shoot();
	uint8_t stopShoot();

	// Settings
	uint8_t setMode(uint8_t option);
	uint8_t setOrientation(uint8_t option);

	// Video
	uint8_t setVideoResolution(uint8_t option);
	uint8_t setVideoFov(uint8_t option);
	uint8_t setFrameRate(uint8_t option);
	uint8_t setVideoEncoding(uint8_t option);

	// Photo
	uint8_t setPhotoResolution(uint8_t option);
	uint8_t setTimeLapseInterval(float option);
	uint8_t setContinuousShot(uint8_t option);

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
	WiFiClient _client;
	String _host = "10.5.5.9";
	uint16_t _port = 80;
	HttpClient _http = HttpClient(_client, _host, _port);

	String _ssid;
	String _pwd;
	uint8_t _camera;

	String _url;
	String _request;
	String _option;

	uint8_t _connected = false;

	HardwareSerial *_debug_port;
	uint8_t _debug;

	uint8_t sendRequest(String request);
};

#endif //GOPRO_CONTROL_H
