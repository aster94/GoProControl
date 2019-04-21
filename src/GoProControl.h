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

// include the correct wifi library
#if defined(ARDUINO_ARCH_ESP32) // ESP32
#include <WiFi.h>
#define INVERT_MAC
#elif defined(ARDUINO_ARCH_ESP8266) // ESP8266
#include <ESP8266WiFi.h>
#define INVERT_MAC
#warning "turnOn() function won't work if you don't provide the mac of your camera"
#elif defined(ARDUINO_SAMD_MKR1000) // MKR1000
#include <WiFi101.h>
#elif defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) // MKR WiFi 1010 and UNO WiFi Rev.2
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKRVIDOR4000) // MKR VIDOR 4000
#include <VidorPeripherals.h>
#include <WiFiNINA.h>
#else // any board (like arduino UNO) without wifi + ESP01 with AT commands
#include <WiFiEsp.h>
#define AT_COMMAND
#warning "Are you using an ESP01 + AT commands? if not open an issue on github: https://github.com/aster94/GoProControl/issues"
#endif

// include the UDP library to turn on and off HERO4 and newer camera
#if defined(AT_COMMAND)
#include <WiFiEspUdp.h>
#define WiFiClient WiFiEspClient
#define WiFiUDP WiFiEspUDP
#else
#include <WiFiUdp.h>
#endif

// include the correct Serial class
#if defined(ARDUINO_ARCH_SAMD)
#define UniversalSerial Serial_
#else
#define UniversalSerial HardwareSerial
#endif

#define MAC_ADDRESS_LENGTH 6

class GoProControl
{
  public:
    // Constructors
    GoProControl(const String ssid, const String pwd, const uint8_t camera, const uint8_t gopro_mac[] = NULL, const String board_name = "");

    // Comunication
    uint8_t begin();
    void end();
    uint8_t keepAlive();

// BLE functions are availables only on ESP32
#if defined(ARDUINO_ARCH_ESP32)
    // none of these function will work, I am adding these for a proof of concept, see the readME
    // https://github.com/KonradIT/goprowifihack/blob/master/HERO5/HERO5-Commands.md#bluetooth-pairing
    uint8_t enableBLE();
    uint8_t disableBLE();
    uint8_t wifiOff();
    uint8_t wifiOn();
#endif

    // Control
    uint8_t turnOn();
    uint8_t turnOff(const bool force = false);
    bool isOn();
    bool isConnected(const bool silent = true);
    bool isRecording();

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
    void enableDebug(UniversalSerial *debug_port, const uint32_t debug_baudrate = 115200);
    void disableDebug();
    void printStatus();

  private:
    WiFiClient _wifi_client;
    WiFiUDP _udp_client;
    const String _host = "10.5.5.9";
    const uint16_t _wifi_port = 80;
    const uint8_t _udp_port = 9;

    String _ssid;
    String _pwd;
    uint8_t _camera;

    String _request;
    uint16_t _response;
    String _parameter;
    String _parameter2;

    uint8_t _mode;

    uint8_t *_gopro_mac = new uint8_t[MAC_ADDRESS_LENGTH];
    uint8_t *_board_mac = new uint8_t[MAC_ADDRESS_LENGTH];
    String _board_name;

    bool WIFI_MODE = true;
    bool BLE_ENABLED = false;

    bool _connected = false;
    bool _recording = false;
    uint64_t _last_request;

    UniversalSerial *_debug_port;
    bool _debug;

    void sendWoL();
    uint8_t sendRequest(const String request);
    uint16_t sendHTTPRequest(const String request);
#if defined(ARDUINO_ARCH_ESP32)
    uint8_t sendBLERequest(const uint8_t request[]);
#endif
    uint8_t connectClient();
    uint8_t confirmPairing();
    uint16_t listenResponse();
    char *splitString(char str[], uint8_t index);
    void printMacAddress(const uint8_t mac[]);
    void getBSSID();
    void revert(uint8_t arr[]);
};

#endif //GOPRO_CONTROL_H
