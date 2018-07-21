#include <GoProControl.h>
String GoProName = "your_gopro_name";
String GoProPassword = "your_gopro_password";
GoProControl gp(GoProName, GoProPassword, HERO3);

uint8_t onStatus = true;
char in = 0;

void setup() {
  Serial.begin(115200);
  gp.enableDebug(true);
  while (!Serial);
  Serial.println("starting");
}

void loop() {
  if (Serial.available() > 0) {
    in = Serial.read();
    //Serial.write(in);
  }

  switch (in) {
    default:
      break;

    //connect
    case 'C':
      Serial.println("trying connection");
      if (!gp.GoProStatus()) {
        gp.begin();
      } else {
        Serial.println("already connected");
      }
      break;

    //turn on/off
    case 'T':
      if (gp.GoProStatus()) {
        Serial.print("turn ");

        onStatus = !onStatus;
        if (onStatus) {
          Serial.println("on");
          if (gp.turnOn())
            Serial.println("did");
        } else {
          Serial.println("off");
          if (gp.turnOff())
            Serial.println("did");
        }
      } else {
        Serial.println("connect the camera first!");
      }
      break;

    //start/stop capture
    case 'A':
      if (gp.GoProStatus()) {
        Serial.print("shoot");
        if (gp.startCapture()) {
          Serial.println("did");
        }
      } else {
        Serial.println("connect the camera first!");
      }
      break;

    case 'S':
      Serial.println("stop");
      if (gp.stopCapture())
        break;

    //set mode
    case 'V':
      gp.setCameraMode(VIDEO_MODE);
      break;

    case 'U':
      gp.setCameraOrientation(ORIENTATION_UP);
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
  }

  in = 0;
  delay(100);
}
