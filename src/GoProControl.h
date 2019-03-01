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

#if defined (ARDUINO_ARCH_ESP8266)
	#include <ESP8266WiFi.h>
	#include <ESP8266HTTPClient.h>
#elif defined (ARDUINO_ARCH_ESP32)
	#include <HTTPClient.h>
#endif


class GoProControl
{
  public:
	
	GoProControl(String ssid, String pwd, uint8_t camera);
	uint8_t begin();
	uint8_t enableDebug(uint8_t debug);	//return debugStatus
	uint8_t GoProStatus();	//return GoProConnected and if the debug is enablad useful info

    uint8_t turnOn(void);
    uint8_t turnOff(void);

    uint8_t startCapture(void);
    uint8_t stopCapture(void);

    uint8_t localizationOn(void);
    uint8_t localizationOff(void);

	uint8_t setCameraMode(uint8_t option);
	uint8_t	setCameraOrientation(uint8_t option);
	uint8_t setVideoResolution(uint8_t option);
	uint8_t setPhotoResolution(uint8_t option);
	uint8_t	setFrameRate(uint8_t option);
	uint8_t setFov(uint8_t option);
	
	uint8_t	setVideoMode(uint8_t option);
    uint8_t setTimeLapseInterval(float option);
    uint8_t setContinuousShot(uint8_t option);

    uint8_t deleteLast(void);
    uint8_t deleteAll(void);
	static void sendWoL(WiFiUDP udp, byte * mac, size_t size_of_mac);
	uint8_t confirmPairing(void);

  private:
  
	WiFiClient client;
	HTTPClient http;

	String url;
	
	String _ssid;
    String _pwd;
	uint8_t _camera;
	String _host;
	uint16_t _port;
	
	uint8_t GoProConnected = false;
	uint8_t debugStatus;

	uint8_t sendRequest(String thisRequest);
    String listen(void);
};

#endif	//GOPRO_CONTROL_H