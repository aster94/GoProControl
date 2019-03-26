#include <GoProControl.h>
#include "Constants.h"

/*
  Control two or more GoPro: to start edit the file Constants.h
*/

GoProControl Hero_Seven(GOPRO_SSID, GOPRO_PASS, CAMERA, gopro_mac_address, BOARD_NAME);
GoProControl Hero_Four(GOPRO_2_SSID, GOPRO_2_PASS, CAMERA_2, gopro_2_mac_address, BOARD_NAME);

void setup()
{
  Hero_Seven.enableDebug(&Serial);
  Hero_Four.enableDebug(&Serial);

  Hero_Four.begin();
  Hero_Seven.begin();
}

void loop()
{
  Hero_Four.shoot();
  delay(1000);
  Hero_Seven.shoot();
  delay(500);
}