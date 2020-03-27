#include <GoProControl.h>
#include "Secrets.h"

/*
  Example with the FreeRTOS framework
*/

GoProControl gp(GOPRO_SSID, GOPRO_PASS, CAMERA);

void setup()
{
  gp.enableDebug(&Serial);
  xTaskCreate(keep_alive, "keep_alive", 10000, NULL, 1, NULL);
}

void loop()
{
  char in = 0;
  if (Serial.available() > 0)
  {
    in = Serial.read();
  }

  switch (in)
  {
  default:
    break;

  // Connect
  case 'C':
    gp.begin();
    break;

  // Turn on and off
  case 'T':
    gp.turnOn();
    break;

  case 't':
    gp.turnOff();
    break;

  // Take a picture of start a video
  case 'A':
    gp.shoot();
    break;

  // Stop the video
  case 'S':
    gp.stopShoot();
    break;

  // Set modes
  case 'V':
    gp.setMode(VIDEO_MODE);
    break;

  case 'P':
    gp.setMode(PHOTO_MODE);
    break;

  case 'M':
    gp.setMode(MULTISHOT_MODE);
    break;

  // Change the orientation
  case 'u':
    gp.setOrientation(ORIENTATION_UP);
    break;

  case 'd':
    gp.setOrientation(ORIENTATION_DOWN);
    break;

  // Change other parameters
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

  // Localize the camera
  case 'O':
    gp.localizationOn();
    break;

  case 'I':
    gp.localizationOff();
    break;

  // Delete some files, be carefull!
  case 'l':
    gp.deleteLast();
    break;

  case 'D':
    gp.deleteAll();
    break;

  // Print useful data
  case 'p':
    gp.printStatus();
    break;

  // Close the connection
  case 'X':
    gp.end();
    break;
  }
}

void keep_alive(void *parameter)
{
  while (1)
  {
    gp.keepAlive();
  }
  vTaskDelete(NULL);
}
