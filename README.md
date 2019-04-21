# GoPro Control Arduino

This is a library to interface with GoPro cameras over WiFi, just press a button and control your GoPro action camera using an Arduino!

![Alt text](/extras/gopro3_and_nodemcu.jpg?raw=true "GoPro3")

![Alt text](/extras/gopros_and_esps.jpg?raw=true "GoPro4")

Have you ever thought about the possibility to control your action camera with your Arduino? Well if you are here because you googled that you may be interested in watching this video

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
- more?

## Supported boards:

- ESP32
- ESP8266
- MKR1000
- MKR WiFi 1010
- MKR VIDOR 4000
- UNO WiFi Rev.2
- any arduino boards (UNO, nano, 101, etc.) attached to an ESP8266 (ESP01) using AT commands with [this library](https://github.com/bportaluri/WiFiEsp)

## Supported cameras:

- HERO3
- HERO4
- HERO5
- HERO6
- HERO7
- FUSION

I made the library with a style which would be quite easy to add other cameras (not only GoPro). I would be very happy to accept pull requests 😃

## Installation

- Arduino IDE:
	- Go to Tools > Manage libraries
	- Search for `GoProControl`
- PlatformIO:
	- From command line: run `pio lib install "GoProControl"`
	- Or if you prefer a GUI from [Platformio IDE](https://docs.platformio.org/en/latest/librarymanager/)
- Manually:
	- `cd $HOME/Arduino/libraries` ([see Arduino library paths for other operating systems](https://www.arduino.cc/en/hacking/libraries))
	- `git clone https://github.com/aster94/GoProControl.git`
	- Restart Arduino IDE

## Examples

**Important:** Rename the `Constants.h.example` to `Constants.h` and change the SSID, password and camera model

## Supported Options

| Mode | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| VIDEO_MODE | ✔ | ✔ |
| PHOTO_MODE | ✔ | ✔ |
| BURST_MODE | ✔  | ✔ |
| TIMELAPSE_MODE | ✔ | ✔ |
| TIMER_MODE | ✔  | ✔ |
| PLAY_HDMI_MODE | ✔ | ✔ |
| MULTISHOT_MODE |  | ✔ |
| VIDEO_SUB_MODE |  | ✔ |
| VIDEO_PHOTO_MODE |  | ✔ |
| VIDEO_TIMELAPSE_MODE |  | ✔ |
| VIDEO_LOOPING_MODE |  | ✔ |
| VIDEO_TIMEWARP_MODE |  | ✔ |
| PHOTO_SINGLE_MODE |  | ✔ |
| PHOTO_NIGHT_MODE |  | ✔ |
| MULTISHOT_BURST_MODE |  | ✔ |
| MULTISHOT_TIMELAPSE_MODE |  | ✔ |
| MULTISHOT_NIGHTLAPSE_MODE |  | ✔ |

| Orientation | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| ORIENTATION_UP | ✔ | ✔ |
| ORIENTATION_DOWN | ✔ | ✔ |
| ORIENTATION_AUTO |  | ✔ |

| Video Resolution | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| VR_4K |  | ✔ |
| VR_2K |  | ✔ |
| VR_2K_SuperView |  | ✔ |
| VR_1440p |  | ✔ |
| VR_1080p_SuperView |  | ✔ |
| VR_1080p | ✔ | ✔ |
| VR_960p | ✔ | ✔ |
| VR_720p_SuperView |  | ✔ |
| VR_720p | ✔ | ✔ |
| VR_WVGA | ✔ | ✔ |

| Field Of View | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| WIDE_FOV | ✔ | ✔ |
| MEDIUM_FOV | ✔ | ✔ |
| NARROW_FOV | ✔ | ✔ |
| LINEAR_FOV |  | ✔ |

| Frame Rate | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| FR_240 | ✔ | ✔ |
| FR_120 | ✔ | ✔ |
| FR_100 | ✔ | ✔ |
| FR_90 |  | ✔ |
| FR_80 |  | ✔ |
| FR_60 | ✔ | ✔ |
| FR_50 | ✔ | ✔ |
| FR_48 | ✔ | ✔ |
| FR_30 | ✔ | ✔ |
| FR_25 | ✔ | ✔ |
| FR_24 | ✔ |  |
| FR_15 | ✔ |  |
| FR_12p5 | ✔ |  |
| FR_12 | ✔ |  |

| Video Encoding | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| NTSC | ✔ | ✔ |
| PAL | ✔ | ✔ |

| Photo Resolution | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| PR_12MP_WIDE |  | ✔ |
| PR_12MP_MEDIUM |  | ✔ |
| PR_12MP_NARROW |  | ✔ |
| PR_12MP_LINEAR |  | ✔ |
| PR_11MP_WIDE | ✔ | ✔ |
| PR_8MP_WIDE | ✔ | ✔ |
| PR_8MP_MEDIUM |  | ✔ |
| PR_7MP_WIDE |  | ✔ |
| PR_7MP_MEDIUM |  | ✔ |
| PR_5MP_WIDE | ✔ | ✔ |
| PR_5MP_MEDIUM |  | ✔ |

| Time Lapse | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| 60 | ✔ | ✔ |
| 30 | ✔ | ✔ |
| 10 | ✔ | ✔ |
| 5 | ✔ | ✔ |
| 1 | ✔ | ✔ |
| 0.5 | ✔ | ✔ |

| Continuous Shot | HERO3 | HERO4,5,6,7 |
| --- | :---: | :---: |
| 10 | ✔ |  |
| 5 | ✔ |  |
| 3 | ✔ |  |
| 0 | ✔ |  |

**NOTE:** Not all the options are available for all the cameras (for example on a HERO3 you can't set 1080p at 240 frame per second 😲). You can see the possibilities on the manual of your camera of here for [HERO3](https://github.com/KonradIT/goprowifihack/blob/master/HERO3/Framerates-Resolutions.md) and here for [HERO4 and newer](https://github.com/KonradIT/goprowifihack/blob/master/HERO4/Framerates-Resolutions.md)

## To Do list and known issues

- ESP8266 can't get the BSSID of the camera. So if you want to turn it on you need to manually pass it to the constructor, [see the docs](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/station-class.html#bssid)
- There is no way to know if an HERO3 camera is on or off so the function `isOn()` will always return `true` on this camera
- It is possible to get a lot of info (mode, fow, battery) from HERO4 and newer camera but this is not implemented, [see here](https://github.com/KonradIT/goprowifihack/blob/master/HERO5/HERO5-Commands.md#gopro-hero5-commands-status-and-notes) - PR are welcome
- BLE not implemented: the ESP32 core is not enough stable, especially, if used together with wifi: [see here](https://github.com/espressif/arduino-esp32/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+ble)
- No confirm pairing for HERO4: [see here](https://github.com/KonradIT/goprowifihack/blob/master/HERO4/WifiCommands.md#code-pairing) - PR are welcome
- The arduino class `String()` is known to cause memory leaks but in hours and hours of use of this library I never had a single problem

## Reference

All the commands came from [here](https://github.com/KonradIT/goprowifihack)

The idea of making a GoPro library for arduino comes from another [library](https://github.com/agdl/GoPRO) which works only on arduino WiFi boards and only with GoPro HERO3