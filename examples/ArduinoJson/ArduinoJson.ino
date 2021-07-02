#include <ArduinoJson.h>
#include <GoProControl.h>
#include "Secrets.h"

// Allocate the JSON document
StaticJsonDocument<4000> status;
StaticJsonDocument<1000> media; // media list
GoProControl gp(GOPRO_SSID, GOPRO_PASS, CAMERA);

void setup()
{
  Serial.begin(115200);
  //gp.enableDebug(&Serial);
}

void compute_status()
{
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(status, gp.getStatus());

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Print values
  // To understand these values see the camera status file, for example for MAX:
  // https://github.com/KonradIT/goprowifihack/blob/master/MAX/CameraStatus.md
  byte battery = status["status"]["1"];
  byte charge = status["status"]["2"];
  byte mode = status["settings"]["1"];
  byte vr = status["settings"]["2"];

  Serial.print("Battery: ");
  Serial.println(battery);
  Serial.print("Charge: ");
  Serial.println(charge);
  Serial.print("Mode: ");
  Serial.println(mode);
  Serial.print("Video Resolution: ");
  Serial.println(vr);
}

void compute_mediaList()
{
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(media, gp.getMediaList());

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Print values.
  const char *id = media["id"];
  const char *folder = media["media"][0]["d"];
  JsonArray array = media["media"][0]["fs"];
  byte pic = array.size();

  Serial.print("ID: ");
  Serial.println(id);
  Serial.print("Folder: ");
  Serial.println(folder);
  Serial.print("Number of pic: ");
  Serial.println(pic);
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
    if (gp.begin() == 1)
    {
      Serial.println("Connected!");
    }
    break;

  case 'c':
    Serial.print("Connected: ");
    Serial.println(gp.isConnected() == true ? "Yes" : "No");
    break;

  case 's':
    Serial.println("Status");
    compute_status();
    break;

  case 'm':
    Serial.println("Media list");
    compute_mediaList();
    break;

  case 'x':
    Serial.println("Closing");
    gp.end();
    break;
  }
  gp.keepAlive();
}
