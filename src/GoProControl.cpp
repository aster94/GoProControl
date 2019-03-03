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

	if (_camera == HERO3) // HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
	{
		// URL scheme: http://HOST/param1/PARAM2?t=PASSWORD&p=%OPTION
		// example:	  http://10.5.5.9/camera/SH?t=password&p=%01

		_host = "10.5.5.9";
		_port = 80;

		_url = "http://" + _host + "/camera/";
	}
	else if (_camera >= HERO4) // HERO4, 5, 6, 7:
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
	if (_camera <= HERO2)
	{
		if (_debug)
		{
			_debug_port->println("Camera not supported");
		}
		return -1;
	}

	char ssid_c[_ssid.length()];
	sprintf(ssid_c, "%s", _ssid.c_str());

	char pwd_c[_pwd.length()];
	sprintf(pwd_c, "%s", _pwd.c_str());

	WiFi.begin(ssid_c, pwd_c);

	uint64_t timeStart = millis();

	while (millis() - timeStart < TIME_OUT_CONNECTION) // WL_IDLE_STATUS it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
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
			_connected = true;
			return true;
		}
	}

	if (_debug)
	{
		_debug_port->print("Time out connection");
	}
	_connected = false;
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
	if (_camera == HERO3)
	{
		_request = _url + "DL?t=" + _pwd;
	}
	else if (_camera >= HERO4)
	{
		_request = _url + "command/wireless/pair/complete?success=1&deviceName=ESPBoard";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::turnOn()
{
	if (_camera == HERO3)
	{
		_request = _url + "PW?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= HERO4)
	{
		//todo
		//send Wake-On-LAN command
	}

	return sendRequest(_request);
}

uint8_t GoProControl::turnOff()
{
	if (_camera == HERO3)
	{
		_request = _url + "PW?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= HERO4)
	{

		_request = _url + "command/system/sleep";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::shoot()
{
	if (_camera == HERO3)
	{
		_request = _url + "SH?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= HERO4)
	{

		_request = _url + "command/shutter?p=1";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::stopShoot()
{
	if (_camera == HERO3)
	{
		_request = _url + "SH?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= HERO4)
	{
		_request = _url + "command/shutter?p=0";
	}

	return sendRequest(_request);
}

////////////////////////////////////////////////////////////
////////                  Settings                  ////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setMode(uint8_t option)
{
	if (_camera == HERO3)
	{
		switch (option)
		{
		case VIDEO_MODE:
			_option = "00";
			break;
		case PHOTO_MODE:
			_option = "01";
			break;
		case BURST_MODE:
			_option = "02";
			break;
		case TIMELAPSE_MODE:
			_option = "03";
			break;
		case TIMER_MODE:
			_option = "04";
			break;
		case PLAY_HDMI_MODE:
			_option = "05";
			break;
		default:
			_debug_port->println("Wrong parameter for setMode");
			return -1;
		}

		_request = _url + "CM?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		//todo: add sub-modes
		switch (option)
		{
		case VIDEO_MODE:
			_option = "0";
			break;
		case PHOTO_MODE:
			_option = "1";
			break;
		case MULTISHOT_MODE:
			_option = "2";
			break;
		default:
			_debug_port->println("Wrong parameter for setMode");
			return -1;
		}

		_request = _url + "command/mode?p=" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setOrientation(uint8_t option)
{
	if (_camera == HERO3)
	{
		switch (option)
		{
		case ORIENTATION_UP:
			_option = "00";
			break;
		case ORIENTATION_DOWN:
			_option = "01";
			break;
		default:
			_debug_port->println("Wrong parameter for setOrientation");
			return -1;
		}

		_request = _url + "UP?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case ORIENTATION_UP:
			_option = "0";
			break;
		case ORIENTATION_DOWN:
			_option = "1";
			break;
		case ORIENTATION_AUTO:
			_option = "2";
			break;
		default:
			_debug_port->println("Wrong parameter for setOrientation");
			return -1;
		}

		_request = _url + "setting/52/" + _option;
	}

	return sendRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Video                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setVideoResolution(uint8_t option)
{
	if (_camera == HERO3)
	{
		switch (option)
		{
		case VR_1080p:
			_option = "06";
			break;
		case VR_960p:
			_option = "05";
			break;
		case VR_720p:
			_option = "03";
			break;
		case VR_WVGA:
			_option = "01";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoResolution");
			return -1;
		}

		_request = _url + "VR?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case VR_4K:
			_option = "1";
			break;
		case VR_2K:
			_option = "4";
			break;
		case VR_2K_SuperView:
			_option = "5";
			break;
		case VR_1440p:
			_option = "7";
			break;
		case VR_1080p_SuperView:
			_option = "8";
			break;
		case VR_1080p:
			_option = "9";
			break;
		case VR_960p:
			_option = "10";
			break;
		case VR_720p_SuperView:
			_option = "11";
			break;
		case VR_720p:
			_option = "12";
			break;
		case VR_WVGA:
			_option = "13";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoResolution");
			return -1;
		}

		_request = _url + "setting/2/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setVideoFov(uint8_t option)
{
	if (_camera == HERO3)
	{
		switch (option)
		{
		case WIDE_FOV:
			_option = "00";
			break;
		case MEDIUM_FOV:
			_option = "01";
			break;
		case NARROW_FOV:
			_option = "02";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoFov");
			return -1;
		}

		_request = _url + "FV?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case WIDE_FOV:
			_option = "0";
			break;
		case MEDIUM_FOV:
			_option = "1";
			break;
		case NARROW_FOV:
			_option = "2";
			break;
		case LINEAR_FOV:
			_option = "4";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoFov");
			return -1;
		}

		_request = _url + "setting/4/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setFrameRate(uint8_t option)
{
	if (_camera == HERO3)
	{
		switch (option)
		{
		case FR_240:
			_option = "0a";
			break;
		case FR_120:
			_option = "09";
			break;
		case FR_100:
			_option = "08";
			break;
		case FR_60:
			_option = "07";
			break;
		case FR_50:
			_option = "06";
			break;
		case FR_48:
			_option = "05";
			break;
		case FR_30:
			_option = "04";
			break;
		case FR_25:
			_option = "03";
			break;
		case FR_24:
			_option = "02";
			break;
		case FR_12p5:
			_option = "0b";
			break;
		case FR_15:
			_option = "01";
			break;
		case FR_12:
			_option = "00";
			break;
		default:
			_debug_port->println("Wrong parameter for setFrameRate");
			return -1;
		}

		_request = _url + "FS?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case FR_240:
			_option = "0";
			break;
		case FR_120:
			_option = "1";
			break;
		case FR_100:
			_option = "2";
			break;
		case FR_90:
			_option = "3";
			break;
		case FR_80:
			_option = "4";
			break;
		case FR_60:
			_option = "5";
			break;
		case FR_50:
			_option = "6";
			break;
		case FR_48:
			_option = "7";
			break;
		case FR_30:
			_option = "8";
			break;
		case FR_25:
			_option = "9";
			break;
		default:
			_debug_port->println("Wrong parameter for setFrameRate");
			return -1;
		}

		_request = _url + "setting/3/" + _option;
	}

	return sendRequest(_request);
}

uint8_t GoProControl::setVideoEncoding(uint8_t option)
{
	if (_camera == HERO3)
	{
		switch (option)
		{
		case NTSC:
			_option = "00";
			break;
		case PAL:
			_option = "01";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoEncoding");
			return -1;
		}

		_request = _url + "VM?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case NTSC:
			_option = "0";
			break;
		case PAL:
			_option = "1";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoEncoding");
			return -1;
		}

		_request = _url + "setting/57/" + _option;
	}

	return sendRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Photo                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setPhotoResolution(uint8_t option)
{
	if (_camera == HERO3)
	{
		switch (option)
		{
		case PR_11MP_WIDE:
			_option = "00";
			break;
		case PR_8MP_WIDE:
			_option = "01";
			break;
		case PR_5MP_WIDE:
			_option = "02";
			break;
		default:
			_debug_port->println("Wrong parameter for setPhotoResolution");
			return -1;
		}

		_request = _url + "PR?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case PR_12MP_WIDE:
			_option = "0";
			break;
		case PR_12MP_LINEAR:
			_option = "10";
			break;
		case PR_12MP_MEDIUM:
			_option = "8";
			break;
		case PR_12MP_NARROW:
			_option = "9";
			break;
		case PR_7MP_WIDE:
			_option = "1";
			break;
		case PR_7MP_MEDIUM:
			_option = "2";
			break;
		case PR_5MP_WIDE:
			_option = "3";
			break;
		default:
			_debug_port->println("Wrong parameter for setPhotoResolution");
			return -1;
		}

		_request = _url + "setting/17/" + _option;
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

	// float cannot be used in switch statements
	if (option == 0.5)
	{
		option = 0;
	}
	uint8_t integer_option = (int)option;

	if (_camera == HERO3)
	{
		switch (integer_option)
		{
		case 60:
			_option = "3c";
			break;
		case 30:
			_option = "1e";
			break;
		case 10:
			_option = "0a";
			break;
		case 5:
			_option = "05";
			break;
		case 1:
			_option = "01";
			break;
		case 0: // 0.5
			_option = "00";
			break;
		default:
			_debug_port->println("Wrong parameter for setTimeLapseInterval");
			return -1;
		}

		_request = _url + "TI?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		switch (integer_option)
		{
		case 60:
			_option = "6";
			break;
		case 30:
			_option = "5";
			break;
		case 10:
			_option = "4";
			break;
		case 5:
			_option = "3";
			break;
		case 1:
			_option = "1";
			break;
		case 0: // 0.5
			_option = "0";
			break;
		default:
			_debug_port->println("Wrong parameter for setTimeLapseInterval");
			return -1;
		}

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

	if (_camera == HERO3)
	{
		switch (option)
		{
		case 10:
			_option = "0a";
			break;
		case 5:
			_option = "05";
			break;
		case 3:
			_option = "03";
			break;
		case 0:
			_option = "00";
			break;
		default:
			_debug_port->println("Wrong parameter for setContinuousShot");
			return -1;
		}

		_request = _url + "CS?t=" + _pwd + "&p=%" + _option;
	}
	else if (_camera >= HERO4)
	{
		// Not supported in Hero4/5/6/7
		return false;
	}
	return sendRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Others                   ////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::localizationOn()
{
	if (_camera == HERO3)
	{
		_request = _url + "LL?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= HERO4)
	{
		_request = _url + "command/system/locate?p=1";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::localizationOff()
{
	if (_camera == HERO3)
	{
		_request = _url + "LL?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= HERO4)
	{
		_request = _url + "command/system/locate?p=0";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::deleteLast()
{
	if (_camera == HERO3)
	{
		_request = _url + "DL?t=" + _pwd;
	}
	else if (_camera >= HERO4)
	{
		_request = _url + "command/storage/delete/last";
	}

	return sendRequest(_request);
}

uint8_t GoProControl::deleteAll()
{
	if (_camera == HERO3)
	{
		_request = _url + "DA?t=" + _pwd;
	}
	else if (_camera >= HERO4)
	{
		_request = _url + "command/command/storage/delete/all";
	}

	return sendRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Debug                   /////////
////////////////////////////////////////////////////////////

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

uint8_t GoProControl::getStatus()
{
	return _connected;
}

void GoProControl::printStatus()
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
		_connected = false;
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

String GoProControl::listenResponse() //never called?
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