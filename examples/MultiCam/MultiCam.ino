#include <GoProControl.h>
#include "Constants.h"

GoProControl Hero_Seven(GOPRO_SSID, GOPRO_PASS, HERO6);
GoProControl Hero_Four(GOPRO_2_SSID, GOPRO_2_PASS, HERO4);

uint8_t onStatus = true;
char in = 0;
byte macAddr[] = {0x78, 0x0C, 0xB8, 0xAE, 0xAE, 0xB1};
WiFiUDP udp;

void setup()
{
  Hero_Seven.enableDebug(&Serial);
  Hero_Four.enableDebug(&Serial);
  udp.begin(9);
}

void loop()
{
  // Shoot
  Hero_Seven.begin();
  delay(4000);
  if (!Hero_Seven.shoot())
  {
    Hero_Seven.begin();
  }
  delay(4000);
  Hero_Four.begin();
  delay(4000);
  Hero_Four.shoot();

  delay(4000);
}