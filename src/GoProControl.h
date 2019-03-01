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
#include <Settings.h>

#include <WiFiUdp.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <HTTPClient.h>
#endif

class GoProControl
{
  public:
	GoProControl(String ssid, String pwd, uint8_t camera);
	uint8_t begin();
	static void sendWoL(WiFiUDP udp, byte *mac, size_t size_of_mac);
	uint8_t confirmPairing();

	void enableDebug(HardwareSerial *debug_port, const uint32_t debug_baudrate = 115200);
	void disableDebug();
	uint8_t getGoProStatus();
	void printGoProStatus();

	uint8_t turnOn();
	uint8_t turnOff();

	uint8_t startCapture();
	uint8_t stopCapture();

	uint8_t localizationOn();
	uint8_t localizationOff();

	uint8_t deleteLast();
	uint8_t deleteAll();

	uint8_t setMode(uint8_t option);
	uint8_t setOrientation(uint8_t option);
	uint8_t setVideoResolution(uint8_t option);
	uint8_t setPhotoResolution(uint8_t option);
	uint8_t setFrameRate(uint8_t option);
	uint8_t setFov(uint8_t option);
	uint8_t setVideoMode(uint8_t option);
	uint8_t setTimeLapseInterval(float option);
	uint8_t setContinuousShot(uint8_t option);

  private:
	WiFiClient _client;
	HTTPClient _http;

	String _url;
	String _ssid;
	String _pwd;

	uint8_t _camera;
	String _host;
	uint16_t _port;

	String _request;
	String _option;

	uint8_t _GoProConnected = false;

	HardwareSerial *_debug_port;
	uint8_t _debug;

	uint8_t sendRequest(String request);
	String listen();
};

#endif //GOPRO_CONTROL_H
