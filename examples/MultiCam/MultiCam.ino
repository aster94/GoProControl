#include <GoProControl.h>
#include <WiFi.h>
#include "Constants.h"
WiFiClient gopro_client;
WiFiUDP udp_client;

GoProControl Hero_Seven(gopro_client, GOPRO_SSID, GOPRO_PASS, CAMERA, udp_client, gopro_mac_address, BOARD_NAME);        // use this if you have a HERO4 or newer
GoProControl Hero_Four(gopro_client, GOPRO_2_SSID, GOPRO_2_PASS, CAMERA_2, udp_client, gopro_2_mac_address, BOARD_NAME); // use this if you have a HERO4 or newer

void setup()
{
  Hero_Seven.enableDebug(&Serial);
  Hero_Four.enableDebug(&Serial);
}

void loop()
{
  Hero_Seven.begin();
  delay(1000);
  if (!Hero_Seven.shoot())
  {
    Hero_Seven.begin();
  }
  delay(1000);
  Hero_Four.begin();
  delay(1000);
  Hero_Four.shoot();

  delay(2000);
}