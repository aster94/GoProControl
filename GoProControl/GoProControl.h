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
#define HERO3+	3
#define HERO3	3
#define HERO4	4
#define HERO5	5
#define HERO6	6

#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266AT.h>	//todo

#define TIME_OUT_CONNECTION 10000

//Screen
#define NTSC 1
#define PAL 0

/*Video Modes*/
#define videoMode 0
#define photoMode 1
#define burstMode 2
#define timelapseMode 3
#define timerMode 4
#define playHDMI 5

/*Orientation*/
#define orientationUp 0
#define orientationDown 1

/*Resolutions*/
#define VRWVGA60 0
#define VRWVGA120 1
#define VRR720_30 2
#define VR720_60 3
#define VR960_30 4
#define VR960_48 5
#define VR1080_30 6

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
#define wideFOV 0
#define mediumFOV 1
#define narrowFOV 2

/*Photo Resolutions*/
#define photo11mpW 0
#define photo8mpW 1
#define photo5mpW 2
#define photo5mpM 3
#define photo12mpW 4
#define photo7mpW 5
#define photo7mpM 6


typedef struct GoPro {
   String  host;
   uint16_t port;
   String url;
};



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

    uint8_t setTimeLapseInterval(float interval);
    uint8_t setContinuousShot(int numberOfShots);
	uint8_t setCameraMode(uint8_t mode);	//todo
    uint8_t setVideoMode(uint8_t mode);

    uint8_t deleteLast(void);
    uint8_t deleteAll(void);

	
  private:
	
	WiFiClient client;
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