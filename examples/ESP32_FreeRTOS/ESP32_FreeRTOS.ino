#include <GoProControl.h>
#include <WiFi.h>
#include "Constants.h"
WiFiClient gopro_client;
WiFiUDP udp_client;

//GoProControl gp(gopro_client, GOPRO_SSID, GOPRO_PASS, CAMERA); // use this if you have a HERO3 or older
GoProControl gp(gopro_client, GOPRO_SSID, GOPRO_PASS, CAMERA, udp_client, gopro_mac_address, BOARD_NAME); // use this if you have a HERO4 or newer

void setup()
{
  gp.enableDebug(&Serial);
  xTaskCreate(keep_alive, "keep_alive", 10000, NULL, 1, NULL);
}

void loop()
{
  // do stuff
}

void keep_alive(void *parameter)
{
  gp.keepAlive();
}