/*
	modified from Arturo Guadalupi: https://github.com/agdl/GoPRO
	by Vincenzo-G: https://www.hackster.io/vincenzo-g
	All right reserved.

	This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
	This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
*/

#include <GoProControl.h>

////////////////////////////////////////////////////////////////////////////
///////////// constructor
////////////////////////////////////////////////////////////////////////////

GoProControl::GoProControl(String ssid, String pwd, uint8_t camera){
	
  	_ssid = ssid;
	_pwd = pwd;
	_camera = camera;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		//URL scheme: http://HOST/param1/PARAM2?t=PASSWORD&p=%OPTION
		//example:	  http://10.5.5.9/camera/SH?t=password&p=%01
		//here we cheate until param1
		
		_host = "10.5.5.9";
		_port = 80;
			
		url = "http://" + _host + "/camera/";
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
}


////////////////////////////////////////////////////////////////////////////
///////////// 
////////////////////////////////////////////////////////////////////////////

uint8_t GoProControl::enableDebug(uint8_t debug) {
  debugStatus = debug;
  return debugStatus;
}

uint8_t GoProControl::GoProStatus(){

	if (debugStatus){
		Serial.print("\nSSID: ");
		Serial.println(WiFi.SSID());
		Serial.print("IP Address: ");
		Serial.println(WiFi.localIP());
		Serial.print("signal strength (RSSI):");
		Serial.print(WiFi.RSSI());
		Serial.println(" dBm\n");
	}
    return GoProConnected;
}

uint8_t GoProControl::begin() {
	
	//first of all check if you are usign a wifi board/module
	if (WiFi.status() == WL_NO_SHIELD) {
		if (debugStatus){
			Serial.println("WiFi board/module not detected");
		}
    
    GoProConnected = false;
    return false;
  	}

	char ssid_c[_ssid.length()];
	sprintf(ssid_c, "%s", _ssid.c_str());

	char pwd_c[_pwd.length()];
	sprintf(pwd_c, "%s", _pwd.c_str());
		
	WiFi.begin(ssid_c, pwd_c);
	
	uint64_t timeStart = millis();
	
	while(millis()-timeStart < TIME_OUT_CONNECTION){
		if (debugStatus){
    		Serial.print("Attempting to connect to SSID: ");
    		Serial.println(_ssid);
			Serial.print("using password: ");
    		Serial.println(_pwd);
		}
    	delay(1500);
		
		if (WiFi.status() == WL_CONNECTED) {
			if (debugStatus){
				Serial.print("Connected");
			}
			
			GoProConnected = true;
			return true;
  		}
 	}
	if (debugStatus){
		Serial.print("Time out connection");
	}
	GoProConnected = false;
	return false;	
}

uint8_t GoProControl::sendRequest(String request) {
  	client.stop();
  	if (!client.connect(_host.c_str(), _port)) {
		if (debugStatus){
    		Serial.println("Connection lost");
    		GoProConnected = false;
		}
    return false;
  	}

	client.print("GET " + request + " HTTP/1.1\r\n");
	client.print("Connection: close\r\n\r\n");

	if (debugStatus){
		Serial.println("my request is:");
		Serial.println("-------------");
		Serial.print("GET " + request + " HTTP/1.1\r\n");
		Serial.print("Connection: close\r\n\r\n");
		Serial.println("-------------\n");
	}
  	if (listen() == "HTTP/1.1 200 OK\r") {
    	return true;
  	}
  	else {
    	return false;
  	}
}

String GoProControl::listen(void) {
	
  	uint64_t startTime = millis();
	String firstLine;
	String response;
	uint8_t end_firstLine = false;

  	while (millis() - startTime < 3000) { //listen for 3 seconds
    	while (client.available()) {
      		char c = client.read();
      		response += c;
			if (end_firstLine == false){
				firstLine += c;
			}
			if (end_firstLine == false && c == '\n'){
				end_firstLine = true;
			}
		}
	}

	if (debugStatus){
	  Serial.println();
	  Serial.print("Received response is:\t");
	  Serial.println(response);
	  Serial.println();
	}
  	return firstLine;
}


////////////////////////////////////////////////////////////////////////////
///////////// MAIN /////////////////////////////
////////////////////////////////////////////////////////////////////////////

uint8_t GoProControl::turnOn(void){
	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "PW?t=" + _pwd + "&p=%01";
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
	return(sendRequest(requestURL));
}

uint8_t GoProControl::turnOff(void){
  	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "PW?t=" + _pwd + "&p=%00";
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
	return(sendRequest(requestURL));
}

uint8_t GoProControl::localizationOn(void){
 	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "LL?t=" + _pwd + "&p=%01";
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
  	return(sendRequest(requestURL));
}

uint8_t GoProControl::localizationOff(void){
	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "LL?t=" + _pwd + "&p=%00";
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
  	return(sendRequest(requestURL));
}

uint8_t GoProControl::startCapture(void){
  	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "SH?t=" + _pwd + "&p=%01";
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

	return(sendRequest(requestURL));
}

uint8_t GoProControl::stopCapture(void){
  	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "SH?t=" + _pwd + "&p=%00";
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

  	return(sendRequest(requestURL));
}

uint8_t GoProControl::deleteLast(void){
  	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "DL?t=" + _pwd;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
  	return(sendRequest(requestURL));
}

uint8_t GoProControl::deleteAll(void){
  	String requestURL;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		requestURL = url + "DA?t=" + _pwd;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
  	return(sendRequest(requestURL));
}


////////////////////////////////////////////////////////////////////////////
///////////// SETTING	///////////////////////
////////////////////////////////////////////////////////////////////////////


uint8_t GoProControl::setCameraMode(uint8_t option){
	String requestURL;
	String stringOption;

	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+

		if(option == VIDEO_MODE) stringOption = "00";
		else if (option == PHOTO_MODE) stringOption = "01";
		else if (option == BURST_MODE) stringOption = "02";
		else if (option == TIMELAPSE_MODE) stringOption = "03";
		else if (option == TIMER_MODE) stringOption = "04";
		else if (option == PLAY_HDMI) stringOption = "05";
		requestURL = url + "CM?t=" + _pwd + "&p=%" + stringOption;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

  	return(sendRequest(requestURL));
}

uint8_t GoProControl::setCameraOrientation(uint8_t option){
	String requestURL;
	String stringOption;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		if(option == ORIENTATION_UP) stringOption = "00";
		else if (option == ORIENTATION_DOWN) stringOption = "01";

		requestURL = url + "UP?t=" + _pwd + "&p=%" + stringOption;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

  	return(sendRequest(requestURL));
}

uint8_t GoProControl::setVideoResolution(uint8_t option){
	String requestURL;
	String stringOption;
	
	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+

		if(option == VR_WVGA60) stringOption = "00";
		else if (option == VR_WVGA120) stringOption = "01";
		else if (option == VR_720_30) stringOption = "02";
		else if (option == VR_720_60) stringOption = "03";
		else if (option == VR_960_30) stringOption = "04";
		else if (option == VR_960_48) stringOption = "05";
		else if (option == VR_1080_30) stringOption = "06";

		requestURL = url + "VR?t=" + _pwd + "&p=%" + stringOption;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

  	return(sendRequest(requestURL));
}

uint8_t GoProControl::setPhotoResolution(uint8_t option){
	String requestURL;
	String stringOption;

	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		if (option == PR_11mpW) stringOption = "00";
		else if (option == PR_8mpW) stringOption = "01";
		else if (option == PR_5mpW) stringOption = "02";

		requestURL = url + "PR?t=" + _pwd + "&p=%" + stringOption;

	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

  	return(sendRequest(requestURL));
}

uint8_t	GoProControl::setFrameRate(uint8_t option){
	String requestURL;
	String stringOption;

	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		if(option == FPS12) stringOption = "00";
		else if (option == FPS15) stringOption = "01";
		else if (option == FPS12p5) stringOption = "0b";
		else if (option == FPS24) stringOption = "02";
		else if (option == FPS25) stringOption = "03";
		else if (option == FPS30) stringOption = "04";
		else if (option == FPS48) stringOption = "05";
		else if (option == FPS50) stringOption = "06";
		else if (option == FPS60) stringOption = "07";
		else if (option == FPS100) stringOption = "08";
		else if (option == FPS120) stringOption = "09";
		else if (option == FPS240) stringOption = "0a";
		requestURL = url + "FS?t=" + _pwd + "&p=%" + stringOption;

	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
  	return(sendRequest(requestURL));
}

uint8_t GoProControl::setFov(uint8_t option){
	String requestURL;
	String stringOption;

	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		if (option == WIDE_FOV) stringOption = "00";
		else if (option == MEDIUM_FOV) stringOption = "01";
		else if (option == NARROW_FOV) stringOption = "02";
		requestURL = url + "FV?t=" + _pwd + "&p=%" + stringOption;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
  	return(sendRequest(requestURL));
}

uint8_t GoProControl::setVideoMode(uint8_t option){
  	String requestURL;
	String stringOption;

	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+

		if(option == NTSC) stringOption = "00";
		else if (option == PAL) stringOption = "01";
		requestURL = url + "VM?t=" + _pwd + "&p=%" + stringOption;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

  	return(sendRequest(requestURL));
}

uint8_t GoProControl::setTimeLapseInterval(float option){
  	String requestURL;
  	String stringOption;

	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		if(option == 0.5) stringOption = "00";
		else if(option == 1) stringOption = "01";
		else if(option == 5) stringOption = "05";
		else if(option == 10) stringOption = "0a";
		else if(option == 30) stringOption = "1e";
		else if(option == 60) stringOption = "3c";
		requestURL = url + "TI?t=" + _pwd + "&p=%" + stringOption;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	

  	return(sendRequest(requestURL));
}

uint8_t GoProControl::setContinuousShot(uint8_t option){
	String requestURL;
	String stringOption;

	if (_camera == 1){			//HERO
		//old not in the todo list
	} else if (_camera == 2) {	//HERO2
		//old not in the todo list
	} else if (_camera == 3 || _camera == 31 || _camera == 32 || _camera == 33) {	//HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
		
		if (option == 0) stringOption = "00";
		else if(option == 3) stringOption = "03";
		else if(option == 5) stringOption = "05";
		else if(option == 10) stringOption = "0a";
		requestURL = url + "CS?t=" + _pwd + "&p=%" + stringOption;
		
	} else if (_camera == 4) {	//HERO4
		//todo
	} else if (_camera == 5) {	//HERO5
		//todo
	} else if (_camera == 6) {	//HERO6
		//todo
	} else if (_camera == 7) {	//next?
		//future
	}	
	
	return(sendRequest(requestURL));
}

////////////////////////////////////////////////////////////////////////////
///////////// end /////////////////////
////////////////////////////////////////////////////////////////////////////
