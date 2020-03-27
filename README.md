# GoPro Control Arduino

This is a library to interface with GoPro cameras over WiFi using Arduino!

![Alt text](/extras/gopro3_and_nodemcu.jpg?raw=true "GoPro3")

![Alt text](/extras/gopros_and_esps.jpg?raw=true "GoPro4")

Here there is a short video that shows the potential of the library

[![Alt text](https://img.youtube.com/vi/PuM-ZQ2tMW0/0.jpg)](https://www.youtube.com/watch?v=PuM-ZQ2tMW0)

## Supported actions

- take a picture
- start and stop a video
- change the mode (photo, video, etc)
- delete last file
- format the SD
- turn the camera on/off
- change the field of view (FOV)
- change frame rate
- change photo and video resolution
- turn the localization on/off
- change the orientation
- more

## Supported boards:

- ESP32
- ESP8266
- MKR1000
- MKR WiFi 1010
- MKR VIDOR 4000
- UNO WiFi Rev.2
- any other boards (UNO, NANO, bluepill) attached to an ESP01 using AT commands with [this library](https://github.com/bportaluri/WiFiEsp)

## Supported cameras:

- HERO3
- HERO4
- HERO5
- HERO6
- HERO7
- FUSION
- HERO8
- MAX

I made the library with a style which would be quite easy to add other cameras (not only GoPro). I would be very happy to accept pull requests 😃

## Installation

- Arduino IDE:
	- Go to Tools > Manage libraries
	- Search for `GoProControl`
- PlatformIO:
	- From command line: run `pio lib install "GoProControl"` or use the ID `pio lib install 5599`
	- Or if you prefer a GUI from [Platformio IDE](https://docs.platformio.org/en/latest/librarymanager/)
- Manually:
	- `cd $HOME/Arduino/libraries` ([see Arduino library paths for other operating systems](https://www.arduino.cc/en/hacking/libraries))
	- `git clone https://github.com/aster94/GoProControl.git`
	- Restart Arduino IDE

## Examples

Start with the [`GoProControl.ino`](examples/GoProControl/GoProControl.ino) to get used with the library

If you wish to control two (or more) camera at the same time check [`MultiCam.ino`](examples/MultiCam/MultiCam.ino)

On the ESP32 there is the possibility to use the dual core architecture with the FreeRTOS framework, check [`ESP32_FreeRTOS.ino`](examples/ESP32_FreeRTOS/ESP32_FreeRTOS.ino)

An advantage use of the `getStatus()` and `getMediaList()` can be seen in [`ArduinoJson.ino`](examples/ArduinoJson/ArduinoJson.ino), you would need to download the `ArduinoJson` library

**Important:** Before uploading to your board you have to change the SSID, password and camera model from `Secrets.h`

## Supported Settings

You can see the available settings in the manual of your camera or [here](https://github.com/KonradIT/goprowifihack/blob/master/HERO3/Framerates-Resolutions.md) for HERO3 and [here](https://github.com/KonradIT/goprowifihack/blob/master/HERO4/Framerates-Resolutions.md) for HERO4 and newer.

In the file [Settings.h](src/Settings.h) you can see how them are defined

**NOTE:** Not all the combination of settings are available for all the cameras (for example on a HERO3 you can't set 8K at 240 frame per second 😲).

## To Do list and known issues

- ESP8266 can't get the BSSID of the camera. So if you want to turn it on you need to manually pass it to the constructor, [see the docs](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/station-class.html#bssid)
- There is no way to know if an HERO3 camera is on or off so the function `isOn()` will always return `true` on this camera - open an issue if you found a way
- BLE not implemented: the ESP32 core is not enough stable, especially if used together with wifi: [see here](https://github.com/espressif/arduino-esp32/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+ble)
- No confirm pairing for HERO4: [see here](https://github.com/KonradIT/goprowifihack/blob/master/HERO4/WifiCommands.md#code-pairing) - PR are welcome
- on STM32 there is a known bug with [WiFiEsp](https://github.com/bportaluri/WiFiEsp/pull/179)
- Arduino official WiFi API doesn't support `getHostname()`, [see here](https://github.com/arduino-libraries/WiFiNINA/issues/57)
- HERO4 and later camera can't be turned on, this is due to a bug inside this library which I wasn't able to fix yet

## Reference

All the commands came from the great work of [KonradIT](https://github.com/KonradIT/goprowifihack)

The idea of making a GoPro library for arduino comes from another [library](https://github.com/agdl/GoPRO) which works only on arduino WiFi boards and only with GoPro HERO3
