/*
GoProControl.cpp

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <GoProControl.h>

GoProControl::GoProControl(String ssid, String pwd, uint8_t camera)
{
	_ssid = ssid;
	_pwd = pwd;
	_camera = camera;

	if (_camera == 3) // HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
	{
		// URL scheme: http://HOST/param1/PARAM2?t=PASSWORD&p=%OPTION
		// example:	  http://10.5.5.9/camera/SH?t=password&p=%01

		_host = "10.5.5.9";
		_port = 80;

		_url = "http://" + _host + "/camera/";
	}
	else if (_camera >= 4) // HERO4, 5, 6, 7:
	{
		// URL scheme: http://HOST/gp/gpControl/....
		// Basic functions (record, mode, tag, poweroff): http://HOST/gp/gpControl/command/PARAM?p=OPTION
		// example change mode to video: http://10.5.5.9/gp/gpControl/command/mode?p=0
		// Settings: http://HOST/gp/gpControl/setting/SETTING/option
		// example change video resolution to 1080p: http://10.5.5.9/gp/gpControl/setting/2/9)

		_host = "10.5.5.9";
		_port = 80;
		_url = "http://" + _host + "/gp/gpControl/";
	}
}

uint8_t GoProControl::begin()
{
	// no support for camera before HERO3
	if (_camera <= 2)
	{
		if (_debug)
		{
			_debug_port->println("Camera not supported");
		}
		return -1;
	}

	// first of all check if you are usign a wifi board/module
	if (WiFi.status() == WL_NO_SHIELD)
	{
		if (_debug)
		{
			_debug_port->println("WiFi board/module not detected");
		}

		_GoProConnected = false;
		return -2;
	}

	char ssid_c[_ssid.length()];
	sprintf(ssid_c, "%s", _ssid.c_str());

	char pwd_c[_pwd.length()];
	sprintf(pwd_c, "%s", _pwd.c_str());

	WiFi.begin(ssid_c, pwd_c);

	uint64_t timeStart = millis();

	while (millis() - timeStart < TIME_OUT_CONNECTION)
	{
		if (_debug)
		{
			_debug_port->print("Attempting to connect to SSID: ");
			_debug_port->println(_ssid);
			_debug_port->print("using password: ");
			_debug_port->println(_pwd);
		}
		delay(1500);

		if (WiFi.status() == WL_CONNECTED)
		{
			if (_debug)
			{
				_debug_port->print("Connected");
			}
			_GoProConnected = true;
			return true;
		}
	}

	if (_debug)
	{
		_debug_port->print("Time out connection");
	}
	_GoProConnected = false;
	return -3;
}

void GoProControl::sendWoL(WiFiUDP udp, byte *mac, size_t size_of_mac)
{
	byte preamble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	byte i;
	IPAddress addr(255, 255, 255, 255);
	udp.begin(9);
	udp.beginPacket(addr, 9); //sending packet at 9,

	udp.write(preamble, sizeof preamble);

	for (i = 0; i < 16; i++)
	{
		udp.write(mac, size_of_mac);
	}
	udp.endPacket();
	delay(2000);
}

uint8_t GoProControl::confirmPairing()
{
	if (_camera == 3)
	{
		_request = _url + "DL?t=" + _pwd;
	}
	else if (_camera >= 4)
	{
		_request = _url + "command/wireless/pair/complete?success=1&deviceName=ESPBoard";
	}

	return sendRequest(_request);
}

void GoProControl::enableDebug(HardwareSerial *debug_port, const uint32_t debug_baudrate)
{
	_debug = true;
	_debug_port = debug_port;
	_debug_port->begin(debug_baudrate);
}

void GoProControl::disableDebug()
{
	_debug_port->end();
	_debug = false;
}

uint8_t GoProControl::getGoProStatus()
{
	return _GoProConnected;
}

void GoProControl::printGoProStatus()
{
	_debug_port->print("\nSSID: ");
	_debug_port->println(WiFi.SSID());
	_debug_port->print("IP Address: ");
	_debug_port->println(WiFi.localIP());
	_debug_port->print("signal strength (RSSI):");
	_debug_port->print(WiFi.RSSI());
	_debug_port->println(" dBm\n");
	//todo add more info like mode (photo, video), fow and so on
}

uint8_t GoProControl::turnOn()
{
	if (_camera == 3)
	{
		_request = _url + "PW?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= 4)
	{
		//todo
		//send Wake-On-LAN command
	}

	return sendRequest(_request);
}

uint8_t GoProControl::turnOff()
{
	if (_camera == 3)
	{
		_request = _url + "PW?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= 4)
	{

		_request = _url + "command/system/sleep";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::startCapture()
{
	if (_camera == 3)
	{
		_request = _url + "SH?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= 4)
	{

		_request = _url + "command/shutter?p=1";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::stopCapture()
{
	if (_camera == 3)
	{
		_request = _url + "SH?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= 4)
	{
		_request = _url + "command/shutter?p=0";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::localizationOn()
{
	if (_camera == 3)
	{
		_request = _url + "LL?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= 4)
	{
		_request = _url + "command/system/locate?p=1";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::localizationOff()
{
	if (_camera == 3)
	{
		_request = _url + "LL?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= 4)
	{
		_request = _url + "command/system/locate?p=0";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::deleteLast()
{
	if (_camera == 3)
	{
		_request = _url + "DL?t=" + _pwd;
	}
	else if (_camera >= 4)
	{
		_request = _url + "command/storage/delete/last";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::deleteAll()
{
	if (_camera == 3)
	{
		_request = _url + "DA?t=" + _pwd;
	}
	else if (_camera >= 4)
	{
		_request = _url + "command/command/storage/delete/all";
	}

	return sendRequest(_request);
}

////////////////////////////////////////////////////////////
////////                  Settings                  ////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setMode(uint8_t option)
{
	if (!validMode(option, mode_first, mode_last))
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setMode");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == VIDEO_MODE)
			_option = "00";
		else if (option == PHOTO_MODE)
			_option = "01";
		else if (option == BURST_MODE)
			_option = "02";
		else if (option == TIMELAPSE_MODE)
			_option = "03";
		else if (option == TIMER_MODE)
			_option = "04";
		else if (option == PLAY_HDMI)
			_option = "05";
		_request = _url + "CM?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		//todo: add sub-modes
		if (option == VIDEO_MODE)
			_option = "0";
		else if (option == PHOTO_MODE)
			_option = "1";
		else if (option == MULTISHOT_MODE)
			_option = "2";
		_request = _url + "command/mode?p=" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setOrientation(uint8_t option)
{
	if (!validMode(option, orientation_first, orientation_last))
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setOrientation");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == ORIENTATION_UP)
			_option = "00";
		else if (option == ORIENTATION_DOWN)
			_option = "01";

		_request = _url + "UP?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		if (option == ORIENTATION_UP)
			_option = "0";
		else if (option == ORIENTATION_DOWN)
			_option = "1";
		else if (option == ORIENTATION_AUTO)
			_option = "2";
		_request = _url + "setting/52/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setVideoResolution(uint8_t option)
{
	if (!validMode(option, video_resolution_first, video_resolution_last))
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setVideoResolution");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == VR_WVGA60)
			_option = "00";
		else if (option == VR_WVGA120)
			_option = "01";
		else if (option == VR_720_30)
			_option = "02";
		else if (option == VR_720_60)
			_option = "03";
		else if (option == VR_960_30)
			_option = "04";
		else if (option == VR_960_48)
			_option = "05";
		else if (option == VR_1080_30)
			_option = "06";

		_request = _url + "VR?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		if (option == VR_4K)
			_option = "1";
		else if (option == VR_2K)
			_option = "4";
		else if (option == VR_2K_SuperView)
			_option = "5";
		else if (option == VR_1440p)
			_option = "7";
		else if (option == VR_1080p_SuperView)
			_option = "8";
		else if (option == VR_1080p)
			_option = "9";
		else if (option == VR_960p)
			_option = "10";
		else if (option == VR_720p_SuperView)
			_option = "11";
		else if (option == VR_720p)
			_option = "12";
		else if (option == VR_WVGA)
			_option = "13";

		_request = _url + "setting/2/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setPhotoResolution(uint8_t option)
{
	if (!validMode(option, photo_resolution_first, photo_resolution_last))
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setPhotoResolution");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == PR_11mpW)
			_option = "00";
		else if (option == PR_8mpW)
			_option = "01";
		else if (option == PR_5mpW)
			_option = "02";

		_request = _url + "PR?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		if (option == PR_12MP_Wide)
			_option = "0";
		else if (option == PR_12MP_Linear)
			_option = "10";
		else if (option == PR_12MP_Medium)
			_option = "8";
		else if (option == PR_12MP_Narrow)
			_option = "9";
		else if (option == PR_7MP_Wide)
			_option = "1";
		else if (option == PR_7MP_Medium)
			_option = "2";
		else if (option == PR_5MP_Wide)
			_option = "3";
		_request = _url + "setting/17/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setFrameRate(uint8_t option)
{
	if (!validMode(option, frame_rate_first, frame_rate_last))
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setFrameRate");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == FPS12)
			_option = "00";
		else if (option == FPS15)
			_option = "01";
		else if (option == FPS12p5)
			_option = "0b";
		else if (option == FPS24)
			_option = "02";
		else if (option == FPS25)
			_option = "03";
		else if (option == FPS30)
			_option = "04";
		else if (option == FPS48)
			_option = "05";
		else if (option == FPS50)
			_option = "06";
		else if (option == FPS60)
			_option = "07";
		else if (option == FPS100)
			_option = "08";
		else if (option == FPS120)
			_option = "09";
		else if (option == FPS240)
			_option = "0a";
		_request = _url + "FS?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		if (option == FR_240)
			_option = "0";
		else if (option == FR_120)
			_option = "1";
		else if (option == FR_100)
			_option = "2";
		else if (option == FR_90)
			_option = "3";
		else if (option == FR_80)
			_option = "4";
		else if (option == FR_60)
			_option = "5";
		else if (option == FR_50)
			_option = "6";
		else if (option == FR_48)
			_option = "7";
		else if (option == FR_30)
			_option = "8";
		else if (option == FR_25)
			_option = "9";
		_request = _url + "setting/3/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setFov(uint8_t option)
{
	if (!validMode(option, fov_first, fov_last))
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setFov");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == WIDE_FOV)
			_option = "00";
		else if (option == MEDIUM_FOV)
			_option = "01";
		else if (option == NARROW_FOV)
			_option = "02";
		_request = _url + "FV?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		if (option == WIDE_FOV)
			_option = "0";
		else if (option == MEDIUM_FOV)
			_option = "1";
		else if (option == NARROW_FOV)
			_option = "2";
		else if (option == LINEAR_FOV)
			_option = "4";

		_request = _url + "setting/4/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setVideoEncoding(uint8_t option)
{
	if (!validMode(option, video_encoding_first, video_encoding_last))
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setVideoEncoding");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == NTSC)
			_option = "00";
		else if (option == PAL)
			_option = "01";
		_request = _url + "VM?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		if (option == NTSC)
			_option = "0";
		else if (option == PAL)
			_option = "1";
		_request = _url + "setting/57/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setTimeLapseInterval(float option)
{
	if (option != 0.5 || option != 1 || option != 5 || option != 10 || option != 30 || option != 60)
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setTimeLapseInterval");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == 0.5)
			_option = "00";
		else if (option == 1)
			_option = "01";
		else if (option == 5)
			_option = "05";
		else if (option == 10)
			_option = "0a";
		else if (option == 30)
			_option = "1e";
		else if (option == 60)
			_option = "3c";
		_request = _url + "TI?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		if (option == 0.5)
			_option = "0";
		else if (option == 1)
			_option = "1";
		else if (option == 2)
			_option = "1";
		else if (option == 5)
			_option = "3";
		else if (option == 10)
			_option = "4";
		else if (option == 30)
			_option = "5";
		else if (option == 60)
			_option = "6";
		_request = _url + "setting/5/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setContinuousShot(uint8_t option)
{
	if (option != 0 || option != 3 || option != 5 || option != 10)
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setContinuousShot");
		}
		return -1;
	}

	if (_camera == 3)
	{
		if (option == 0)
			_option = "00";
		else if (option == 3)
			_option = "03";
		else if (option == 5)
			_option = "05";
		else if (option == 10)
			_option = "0a";
		_request = _url + "CS?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= 4)
	{
		// Not supported in Hero4/5/6/7
		return false;
	}
	return sendRequest(_request);
}

////////////////////////////////////////////////////////////
////////               Communication               /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::sendRequest(String request)
{
	_client.stop();
	if (!_client.connect(_host.c_str(), _port))
	{
		if (_debug)
		{
			_debug_port->println("Connection lost");
		}
		_GoProConnected = false;
		return false;
	}

	_http.begin(request);
	int httpCode = _http.GET();
	if (httpCode > 0)
	{
		String payload = _http.getString();
		_debug_port->println(payload);
		return true;
	}
	else
	{
		return false;
	}

	_http.end();
}

String GoProControl::listen()
{
	uint64_t startTime = millis();
	String firstLine;
	String response;
	uint8_t end_firstLine = false;

	while ((millis() - startTime < 3000) /* && (end_firstline == false) */) //listen for 3 seconds
	{
		while (_client.available())
		{
			char c = _client.read();
			response += c;
			if (end_firstLine == false)
			{
				firstLine += c;
			}
			if (end_firstLine == false && c == '\n')
			{
				end_firstLine = true;
			}
		}
	}

	if (_debug)
	{
		_debug_port->println();
		_debug_port->print("Received response is:\t");
		_debug_port->println(response);
		_debug_port->println();
	}
	return firstLine;
}

uint8_t GoProControl::validMode(int16_t mode, int16_t first, int16_t last)
{
	for (int16_t i = first; i <= last; i++)
	{
		if (mode == i)
			return true;
	}
	return false;
}