#include <GoProControl.h>
#include "Constants.h"

// Choose your camera
//GoProControl gp(GOPRO_SSID, GOPRO_PASS, CAMERA); // use this if you have a HERO3 or older
//GoProControl gp(GOPRO_SSID, GOPRO_PASS, CAMERA, gopro_mac_address, BOARD_NAME);

char in = 0;

void setup()
{
  gp.enableDebug(&Serial);
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

  // connect
  case 'C':
    gp.begin();
    break;

  // turn on/off
  case 'T':
    gp.turnOn();
    break;

  case 't':
    gp.turnOff();
    break;

  // take a picture of start a video
  case 'A':
    gp.shoot();
    break;

  // stop the video
  case 'S':
    gp.stopShoot();
    break;

  //set modes
  case 'V':
    gp.setMode(VIDEO_MODE);
    break;

  case 'P':
    gp.setMode(PHOTO_MODE);
    break;

  case 'M':
    gp.setMode(MULTISHOT_MODE);
    break;

  // set orientation
  case 'u':
    gp.setOrientation(ORIENTATION_UP);
    break;

  case 'd':
    gp.setOrientation(ORIENTATION_DOWN);
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

  case 'l':
    gp.deleteLast();
    break;

  case 'D':
    gp.deleteAll();
    break;

  case 'X':
    gp.end();
    break;

  case 'p':
    gp.printStatus();
    break;

  case 'K':
    gp.keepAlive();
    break;
  }

  in = 0;
  gp.keepAlive();
}
