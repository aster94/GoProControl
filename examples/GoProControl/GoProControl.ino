#include <GoProControl.h>
#include <WiFi.h>
#include "Constants.h"

WiFiClient gopro_client;
GoProControl gp(gopro_client, GOPRO_SSID, GOPRO_PASS, CAMERA);

uint8_t onStatus = true;
char in = 0;

byte macAddr[] = {0x06, 0x41, 0x69, 0x91, 0x08, 0xBA};

WiFiUDP udp;

void setup()
{
  gp.enableDebug(&Serial);
  udp.begin(9);
}

void loop()
{
  if (Serial.available() > 0)
  {
    in = Serial.read();
    Serial.write(in);
  }

  switch (in)
  {
  default:
    break;

  //connect
  case 'C':
    Serial.println("trying connection");
    if (!gp.getStatus())
    {
      gp.begin();
    }
    else
    {
      Serial.println("already connected");
    }
    break;

  //turn on/off
  case 'T':
    if (gp.getStatus())
    {
      Serial.print("turn ");

      onStatus = !onStatus;
      if (onStatus)
      {
        Serial.println("on");
        gp.sendWoL(udp, macAddr, sizeof macAddr);
        if (gp.turnOn())
          Serial.println("did");
      }
      else
      {
        Serial.println("off");
        if (gp.turnOff())
          Serial.println("did");
      }
    }
    else
    {
      Serial.println("connect the camera first!");
    }
    break;

  //start/stop capture
  case 'A':
    if (gp.getStatus())
    {
      Serial.print("shoot");
      if (gp.shoot())
      {
        Serial.println("did");
      }
    }
    else
    {
      Serial.println("connect the camera first!");
    }
    break;

  case 'S':
    Serial.println("stop");
    if (gp.stopShoot())
      break;

  //set mode
  case 'V':
    gp.setMode(VIDEO_MODE);
    break;

  case 'P':
    gp.setMode(PHOTO_MODE);
    break;

  case 'M':
    gp.setMode(MULTISHOT_MODE);
    break;

  case 'U':
    gp.setOrientation(ORIENTATION_UP);
    break;

  case 'W':
    gp.setVideoFov(MEDIUM_FOV);
    break;

  case 'E':
    gp.setFrameRate(FR_120);
    break;

  case 'f':
    gp.setPhotoResolution(PR_11MP_WIDE);
    break;

  case 'F':
    gp.setVideoResolution(VR_1080p);
    break;

  case 'L':
    gp.setTimeLapseInterval(60);
    break;

  case 'O':
    gp.localizationOn();
    break;

  case 'I':
    gp.localizationOff();
    break;

  case 'd':
    gp.deleteLast();
    break;

  case 'D':
    gp.deleteAll();
    break;

  case 'p':
    gp.confirmPairing();
    break;
  }

  in = 0;
  delay(100);
}