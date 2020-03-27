#include <GoProControl.h>
#include "Secrets.h"

/*
  Control your GoPro with the Serial Monitor
  edit the file Secrets.h with your camera netword name and password
  CAMERA could be: HERO3, HERO4, HERO5, HERO6, HERO7, FUSION, HERO8, MAX
*/

GoProControl gp(GOPRO_SSID, GOPRO_PASS, CAMERA);

void setup()
{
  gp.enableDebug(&Serial);
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
  case 'b':
    gp.begin();
    break;

  case 'c':
    Serial.print("Connected: ");
    Serial.println(gp.isConnected() == true ? "Yes" : "No");
    break;

  case 'p':
    gp.confirmPairing();
    break;

  case 's':
    Serial.println("Status:");
    gp.getStatus();
    break;

  case 'm':
    Serial.println("Media List:");
    gp.getMediaList();
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

  // Check if it is recording
  case 'r':
    Serial.print("Recording: ");
    Serial.println(gp.isRecording() == true ? "Yes" : "No");
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
  case 'U':
    gp.setOrientation(ORIENTATION_UP);
    break;

  case 'D':
    gp.setOrientation(ORIENTATION_DOWN);
    break;

  // Change other parameters
  case 'f':
    gp.setVideoFov(MEDIUM_FOV);
    break;

  case 'F':
    gp.setFrameRate(FR_120);
    break;

  case 'R':
    gp.setVideoResolution(VR_1080p);
    break;

  case 'h':
    gp.setPhotoResolution(PR_12MP_WIDE);
    break;

  case 'L':
    gp.setTimeLapseInterval(60);
    break;

  // Localize the camera
  case 'O':
    gp.localizationOn();
    break;

  case 'o':
    gp.localizationOff();
    break;

  // Delete some files, be carefull!
  case 'l':
    gp.deleteLast();
    break;

  case 'g':
    gp.deleteAll();
    break;

  // Print useful data
  case 'd':
    gp.printStatus();
    break;

  // Close the connection
  case 'X':
    gp.end();
    break;
  }
  gp.keepAlive(); // not needed on HERO3
}
