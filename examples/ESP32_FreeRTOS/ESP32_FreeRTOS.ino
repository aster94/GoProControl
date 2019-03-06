#include <GoProControl.h>
#include <WiFi.h>
#include "Constants.h"

WiFiClient gopro_client;
GoProControl gp(gopro_client, GOPRO_SSID, GOPRO_PASS, CAMERA);

uint8_t onStatus = true;
char in = 0;

void setup()
{
  gp.enableDebug(&Serial);
  gp.begin();

  xTaskCreate(keep_alive, "keep_alive", 10000, NULL, 1, NULL);
}

void loop()
{
  delay(1000);
}

void keep_alive(void *parameter)
{
  gp.keepAlive();
  delay(1000);
}
