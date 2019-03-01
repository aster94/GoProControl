#include <GoProControl.h>
#include "Constants.h"

GoProControl gp(GOPRO_SSID, GOPRO_PASS, HERO7);

uint8_t onStatus = true;
char in = 0;

byte macAddr[] = {0x06, 0x41, 0x69, 0x91, 0x08, 0xBA};

WiFiUDP udp;

void setup()
{
  Serial.begin(115200);
  gp.enableDebug(&Serial);
  while (!Serial)
    ;
  Serial.println("starting");
  udp.begin(9);
}

void loop()
{
  if (Serial.available() > 0)
  {
    in = Serial.read();
    //Serial.write(in);
  }

  switch (in)
  {
  default:
    break;

  //connect
  case 'C':
    Serial.println("trying connection");
    if (!gp.getGoProStatus())
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
    if (gp.getGoProStatus())
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
    if (gp.getGoProStatus())
    {
      Serial.print("shoot");
      if (gp.startCapture())
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
    if (gp.stopCapture())
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
    gp.setFov(MEDIUM_FOV);
    break;

  case 'E':
    gp.setFrameRate(FPS120);
    break;

  case 'f':
    gp.setPhotoResolution(PR_11mpW);
    break;

  case 'F':
    gp.setVideoResolution(VR_1080_30);
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
