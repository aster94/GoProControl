#include <GoProControl.h>
String GoProName = "your_camera_name";
String GoProPassword = "your_camera_password";
GoProControl gp(GoProName, GoProPassword, HERO3);

uint8_t onStatus = true;
bool captureStatus = false;
byte in;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  gp.enableDebug(true);
  while (!Serial);
  Serial.println("starting");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    in = Serial.read();
    //Serial.write(in);
  }

  switch (in) {
    default: break;

    case 'C':
      Serial.println("trying connection");
      if (!gp.GoProStatus()) {
        gp.begin();
      } else {
        Serial.println("already connected");
      }
      break;

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

    case 'A':
      if (gp.GoProStatus()) {
        Serial.print("capture ");

        captureStatus = !captureStatus;
        if (captureStatus) {
          Serial.println("start");
          if (gp.startCapture())
            Serial.println("did");
        } else {
          Serial.println("stop");
          if (gp.stopCapture())
            Serial.println("did");
        }
      } else {
        Serial.println("connect the camera first!");
      }

      break;
  }
  in = 0;
  delay(100);
}
