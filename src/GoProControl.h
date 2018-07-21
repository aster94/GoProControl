/*
	modified from Arturo Guadalupi: https://github.com/agdl/GoPRO
	by Vincenzo-G: https://www.hackster.io/vincenzo-g
	All right reserved.

	This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
	This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
*/

#ifndef GOPRO_CONTROL_H
#define GOPRO_CONTROL_H

#define HERO	1
#define HERO2	2
#define HERO3	3
#define HERO3P	31	//HERO3+
#define HERO3B	32	//HERO3BLACK
#define HERO3BP	33	//HERO3BLACK+
#define HERO4	4
#define HERO5	5
#define HERO6	6

#include <Arduino.h>

#if defined (ARDUINO_ARCH_ESP8266)
	#include "ESP8266WiFi.h"
#else
	#define ESP_AT
	#include "WiFiEsp.h"
#endif

#define TIME_OUT_CONNECTION 5000

//Screen
#define NTSC 1
#define PAL 0

//Modes
#define VIDEO_MODE 0
#define PHOTO_MODE 1
#define BURST_MODE 2
#define TIMELAPSE_MODE 3
#define TIMER_MODE 4
#define PLAY_HDMI 5

/*Orientation*/
#define ORIENTATION_UP 0
#define ORIENTATION_DOWN 1

/*Resolutions*/
#define VR_WVGA60 0
#define VR_WVGA120 1
#define VR_720_30 2
#define VR_720_60 3
#define VR_960_30 4
#define VR_960_48 5
#define VR_1080_30 6

/*Frame Rates*/
#define FPS12 0
#define FPS15 1
#define FPS12p5 2
#define FPS24 3
#define FPS25 4
#define FPS30 5
#define FPS48 6
#define FPS50 7
#define FPS60 8
#define FPS100 9
#define FPS120 10
#define FPS240 11

/*FOV*/
#define WIDE_FOV 0
#define MEDIUM_FOV 1
#define NARROW_FOV 2

/*Photo Resolutions*/
#define PR_12mpW 0
#define PR_11mpW 1
#define PR_8mpW 2
#define PR_7mpW 3
#define PR_5mpW 4


class GoProControl{
	
  public:
	
	GoProControl(String ssid, String pwd, uint8_t camera);
	uint8_t begin();
	uint8_t enableDebug(uint8_t debug);	//return debugStatus
	uint8_t GoProStatus();	//return GoProConnected and if the debug is enablad useful info

    uint8_t turnOn(void);
    uint8_t turnOff(void);

    uint8_t startCapture(void);
    uint8_t stopCapture(void);

    uint8_t localizationOn(void);
    uint8_t localizationOff(void);

	uint8_t setCameraMode(uint8_t option);
	uint8_t	setCameraOrientation(uint8_t option);
	uint8_t setVideoResolution(uint8_t option);
	uint8_t setPhotoResolution(uint8_t option);
	uint8_t	setFrameRate(uint8_t option);
	uint8_t setFov(uint8_t option);
	
	uint8_t	setVideoMode(uint8_t option);
    uint8_t setTimeLapseInterval(float option);
    uint8_t setContinuousShot(uint8_t option);

    uint8_t deleteLast(void);
    uint8_t deleteAll(void);

	
  private:
#if defined (ESP_AT)
	WiFiEspClient client;
#else
	WiFiClient client;
#endif
	String url;
	
	String _ssid;
    String _pwd;
	uint8_t _camera;
	String _host;
	uint16_t _port;
	
	uint8_t GoProConnected = false;
	uint8_t debugStatus;

	uint8_t sendRequest(String thisRequest);
    String listen(void);
};

#endif	//GOPRO_CONTROL_H
