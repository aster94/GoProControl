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
		//todo
	} else if (_camera == 2) {	//HERO2
		//todo
	} else if (_camera == 3) {	//HERO3
		
		//URL scheme: http://HOST/param1/PARAM2?t=PASSWORD&p=%OPTION
		//example:	  http://10.5.5.9/camera/SH?t=password&p=%01
		//here we cheate until param1
		
		_host = "10.5.5.9";
		_port = 80;
			
		url = "GET http://" + _host + "/camera/";
		
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

	client.print(request);
	client.print("Connection: close\r\n\r\n");

	if (debugStatus){
		Serial.println("my request is:");
		Serial.println("-------------");
	  	Serial.print(request);
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
///////////// 
////////////////////////////////////////////////////////////////////////////


uint8_t GoProControl::turnOn(void){
	String requestURL;
	requestURL = url + "PW?t=" + _pwd + "&p=%01 HTTP/1.1";
	return(sendRequest(requestURL));
}

uint8_t GoProControl::turnOff(void){
  	String requestURL;
	requestURL = url + "PW?t=" + _pwd + "&p=%00 HTTP/1.1";
	return(sendRequest(requestURL));
}

uint8_t GoProControl::localizationOn(void){
  String requestURL;
/*
  if(cameraOrBacpac == CAMERA){
    requestURL = "GET " + cameraURL + "LL?t=" + pwd + "&p=\%01" + " HTTP/1.1";
  }
  else{
    requestURL = "GET " + bacpacURL + "LL?t=" + pwd + "&p=\%01" + " HTTP/1.1";
  }
*/
  return(sendRequest(requestURL));
}

uint8_t GoProControl::localizationOff(void){
  String requestURL;
/*
  if(cameraOrBacpac == CAMERA){
    requestURL = "GET " + cameraURL + "LL?t=" + pwd + "&p=\%00" + " HTTP/1.1";
  }
  else{
    requestURL = "GET " + bacpacURL + "LL?t=" + pwd + "&p=\%00" + " HTTP/1.1";
  }
*/
  return(sendRequest(requestURL));
}

uint8_t GoProControl::startCapture(void){
  	String requestURL;
	requestURL = url + "SH?t=" + _pwd + "&p=%01 HTTP/1.1";
	return(sendRequest(requestURL));
}

uint8_t GoProControl::stopCapture(void){
  	String requestURL;
	requestURL = url + "SH?t=" + _pwd + "&p=%00 HTTP/1.1";
  	return(sendRequest(requestURL));
}


////////////////////////////////////////////////////////////////////////////
///////////// 
////////////////////////////////////////////////////////////////////////////

uint8_t GoProControl::setTimeLapseInterval(float interval){
  String requestURL;
  String stringInterval;

  if(interval == 0.5) stringInterval = "00";
  else if(interval == 1) stringInterval = "01";
  else if(interval == 5) stringInterval = "05";
  else if(interval == 10) stringInterval = "0a";
  else if(interval == 30) stringInterval = "0e";
  else stringInterval = "0c";
/*
  if(cameraOrBacpac == CAMERA){
    requestURL = "GET " + cameraURL + "TI?t=" + pwd + "&p=\%" + stringInterval + " HTTP/1.1";
  }
  else{
    requestURL = "GET " + bacpacURL + "TI?t=" + pwd + "&p=\%" +stringInterval + " HTTP/1.1";
  }
*/
  return(sendRequest(requestURL));
}

uint8_t GoProControl::setContinuousShot(int numberOfShots){
  String requestURL;
  String stringShots;

  if(numberOfShots == 0) stringShots = "00";
  else if(numberOfShots == 3) stringShots = "03";
  else if(numberOfShots == 5) stringShots = "05";
  else stringShots = "0a";
/*
  if(cameraOrBacpac == CAMERA){
    requestURL = "GET " + cameraURL + "CS?t=" + pwd + "&p=\%" + stringShots + " HTTP/1.1";
  }
  else{
    requestURL = "GET " + bacpacURL + "CS?t=" + pwd + "&p=\%" + stringShots + " HTTP/1.1";
  }
*/
  return(sendRequest(requestURL));
}

uint8_t GoProControl::setVideoMode(uint8_t mode){
  String requestURL;

  if(mode == NTSC){
	  
	  //"00";
  }
  else if (mode == PAL) { 
	  //"01";	
  }
/*
  if(cameraOrBacpac == CAMERA){
    requestURL = "GET " + cameraURL + "CS?t=" + pwd + "&p=\%" + videoMode + " HTTP/1.1";
  }
  else{
    requestURL = "GET " + bacpacURL + "CS?t=" + pwd + "&p=\%" +videoMode + " HTTP/1.1";
  }
*/
  return(sendRequest(requestURL));
}

uint8_t GoProControl::deleteLast(void){
  String requestURL;
/*
  if(cameraOrBacpac == CAMERA){
    requestURL = "GET " + cameraURL + "DL?t=" + pwd + " HTTP/1.1";
  }
  else{
    requestURL = "GET " + bacpacURL + "DL?t=" + pwd + " HTTP/1.1";
  }
*/
  return(sendRequest(requestURL));
}

uint8_t GoProControl::deleteAll(void){
  String requestURL;
/*
  if(cameraOrBacpac == CAMERA){
    requestURL = "GET " + cameraURL + "DA?t=" + pwd + " HTTP/1.1";
  }
  else{
    requestURL = "GET " + bacpacURL + "DA?t=" + pwd + " HTTP/1.1";
  }
*/
  return(sendRequest(requestURL));
}

////////////////////////////////////////////////////////////////////////////
///////////// end
////////////////////////////////////////////////////////////////////////////