#include <GoProControl.h>
#include "Secrets.h"

/*
  Control two or more GoPro
*/

GoProControl Hero_Four(GOPRO_1_SSID, GOPRO_1_PASS, YOUR_CAMERA_1);
GoProControl Hero_Seven(GOPRO_2_SSID, GOPRO_2_PASS, YOUR_CAMERA_2);

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
