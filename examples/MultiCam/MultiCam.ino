#include <GoProControl.h>
#include "Constants.h"

GoProControl Hero_Seven(GOPRO_SSID, GOPRO_PASS, CAMERA, gopro_mac_address, BOARD_NAME);
GoProControl Hero_Four(GOPRO_2_SSID, GOPRO_2_PASS, CAMERA_2, gopro_2_mac_address, BOARD_NAME);

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