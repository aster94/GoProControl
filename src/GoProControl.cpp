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

////////////////////////////////////////////////////////////
////////                Constructors                ////////
////////////////////////////////////////////////////////////

GoProControl::GoProControl(const String ssid, const String pwd, const uint8_t camera)
{
	_ssid = ssid;
	_pwd = pwd;
	_camera = camera;

	if (_camera == HERO3) // HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
	{
		// URL scheme: http://HOST/param1/PARAM2?t=PASSWORD&p=%OPTION
		// example:	  http://10.5.5.9/camera/SH?t=password&p=%01

		_url = "http://" + _host + "/camera/";
	}
	else if (_camera >= HERO4) // HERO4, 5, 6, 7:
	{
		// URL scheme: http://HOST/gp/gpControl/....
		// Basic functions (record, mode, tag, poweroff): http://HOST/gp/gpControl/command/PARAM?p=OPTION
		// example change mode to video: http://10.5.5.9/gp/gpControl/command/mode?p=0
		// Settings: http://HOST/gp/gpControl/setting/option
		// example change video resolution to 1080p: http://10.5.5.9/gp/gpControl/setting/2/9)

		_url = "http://" + _host + "/gp/gpControl/";
	}
}

GoProControl::GoProControl(const String ssid, const String pwd, const uint8_t camera, const byte mac_address[], const String board_name)
{
	_ssid = ssid;
	_pwd = pwd;
	_camera = camera;

	if (_camera == HERO3) // HERO3, HERO3+, HERO3BLACK, HERO3BLACK+
	{
		// URL scheme: http://HOST/param1/PARAM2?t=PASSWORD&p=%OPTION
		// example:	  http://10.5.5.9/camera/SH?t=password&p=%01

		_url = "http://" + _host + "/camera/";
	}
	else if (_camera >= HERO4) // HERO4, 5, 6, 7:
	{
		// URL scheme: http://HOST/gp/gpControl/....
		// Basic functions (record, mode, tag, poweroff): http://HOST/gp/gpControl/command/PARAM?p=OPTION
		// example change mode to video: http://10.5.5.9/gp/gpControl/command/mode?p=0
		// Settings: http://HOST/gp/gpControl/setting/option
		// example change video resolution to 1080p: http://10.5.5.9/gp/gpControl/setting/2/9)

		_url = "http://" + _host + "/gp/gpControl/";
	}
	memcpy(_mac_address, mac_address, sizeof(mac_address));
	_board_name = board_name;
}

////////////////////////////////////////////////////////////
////////               Communication               /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::begin()
{
	if (checkConnection())
	{
		if (_debug)
		{
			_debug_port->println("Already connected");
		}
		return false;
	}

	if (_camera <= HERO2)
	{
		if (_debug)
		{
			_debug_port->println("Camera not supported");
		}
		return -1;
	}

	if (_debug)
	{
		_debug_port->println("Attempting to connect to SSID: " + _ssid);
		_debug_port->println("using password: " + _pwd);
	}

	uint8_t attempt = 1;
	while (attempt <= 3) // try three times
	{
		if (_debug)
		{
			_debug_port->print("Attempt: ");
			_debug_port->println(attempt);
		}

		WiFi.begin(_ssid.c_str(), _pwd.c_str());

		while (WiFi.status() == WL_IDLE_STATUS)
		{
			;
		}

		if (WiFi.status() == WL_CONNECTED)
		{
			if (_debug)
			{
				_debug_port->println("Connected to GoPro");
			}
			_connected = true;
			return true;
		}
		else if (WiFi.status() == WL_CONNECT_FAILED)
		{
			if (_debug)
			{
				_debug_port->println("Connection failed");
			}
			_connected = false;
		}
		else
		{
			_debug_port->print("WiFi.status(): ");
			_debug_port->println(WiFi.status());
			_connected = false;
		}
		attempt++;
		delay(200);
	}
	return -(WiFi.status());
}

void GoProControl::end()
{
	if (!checkConnection())
	{
		return;
	}

	if (_debug)
	{
		_debug_port->println("Closing connection");
	}
	_wifi_client.stop();
	WiFi.disconnect();
	_connected = false;
}

uint8_t GoProControl::keepAlive()
{
	if (!checkConnection(true)) // camera not connected
	{
		return false;
	}

	if (millis() - _last_request <= KEEP_ALIVE) // we made a request not so much earlier
	{
		return false;
	}
	else // time to ask something to the camera
	{
		if (_camera == HERO3)
		{
			// not really needed since the connection won't be closed by the camera (tested 6 minutes)
			return true;
		}
		else if (_camera >= HERO4)
		{
			if (_debug)
			{
				_debug_port->println("Keeping connection alive");
			}
			sendRequest("_GPHD_:0:0:2:0.000000\n");
			// should it answer?
		}
	}
}

////////////////////////////////////////////////////////////
////////                    BLE                    /////////
////////////////////////////////////////////////////////////

#if defined(ARDUINO_ARCH_ESP32)
// none of these function will work, I am adding these for a future release
// https://github.com/KonradIT/goprowifihack/blob/master/HERO5/HERO5-Commands.md#bluetooth-pairing
uint8_t GoProControl::enableBLE()
{
	if (_camera <= HERO3)
	{
		if (_debug)
		{
			_debug_port->println("Your camera doesn't have Bluetooth");
		}
		return false;
	}
	BLE_ENABLED = true;
}

uint8_t GoProControl::disableBLE()
{
	if (_camera <= HERO3)
	{
		if (_debug)
		{
			_debug_port->println("Your camera doesn't have Bluetooth");
		}
		return false;
	}
	BLE_ENABLED = false;
}

uint8_t GoProControl::wifiOff()
{
	if (_camera <= HERO3)
	{
		if (_debug)
		{
			_debug_port->println("Your camera doesn't have Bluetooth");
		}
		return false;
	}

	if (BLE_ENABLED == false) // prevent stupid error like turn off the wifi while we didn't connected to the BL yet
	{
		if (_debug)
		{
			_debug_port->println("First run enableBLE()");
		}
		return false;
	}

	WIFI_MODE = false;
	sendBLERequest(BLE_WiFiOff);
}

uint8_t GoProControl::wifiOn()
{
	if (_camera <= HERO3)
	{
		if (_debug)
		{
			_debug_port->println("Your camera doesn't have Bluetooth");
		}
		return false;
	}
	WIFI_MODE = true;
	sendBLERequest(BLE_WiFiOn);
}
#endif

////////////////////////////////////////////////////////////
////////                  Control                  /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::turnOn()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (_camera >= HERO4) // avoid this check in older cameras
	{
		if (isOn()) // camera is already on
		{
			if (_debug)
			{
				_debug_port->println("Camera is already on");
			}
			return false;
		}
	}

	String request;

	if (_camera == HERO3)
	{
		request = _url + "PW?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= HERO4)
	{
		sendWoL();
		return true;
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::turnOff()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (_camera >= HERO4) // avoid this check in older cameras
	{
		if (!isOn()) // camera is already off
		{
			if (_debug)
			{
				_debug_port->println("Camera is already off");
			}
			return false;
		}
	}

	String request;

	if (_camera == HERO3)
	{
		request = _url + "PW?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= HERO4)
	{
		request = _url + "command/system/sleep";
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::isOn()
{
	if (!checkConnection(true)) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	String request;

	if (_camera == HERO3)
	{
		// this isn't supported by this camera so this function will always return true
		return true;
	}
	else if (_camera >= HERO4)
	{
		request = _url + "status";
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::checkConnection(const uint8_t silent)
{
	if (getStatus())
	{
		if (_debug && silent == false)
		{
			_debug_port->println("\nCamera connected");
		}
		return true;
	}
	else
	{
		if (_debug && silent == false)
		{
			_debug_port->println("\nNot connected");
		}
		return false;
	}
}

////////////////////////////////////////////////////////////
////////                   Shoot                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::shoot()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	if (WIFI_MODE)
	{
		String request;

		if (_camera == HERO3)
		{
			request = _url + "SH?t=" + _pwd + "&p=%01";
		}
		else if (_camera >= HERO4)
		{
			request = _url + "command/shutter?p=1";
		}

		return sendHTTPRequest(request);
	}
	else // BLE
	{
#if defined(ARDUINO_ARCH_ESP32)
		sendBLERequest(BLE_RecordStart);
#endif
	}
}

uint8_t GoProControl::stopShoot()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	if (WIFI_MODE)
	{
		String request;

		if (_camera == HERO3)
		{
			request = _url + "SH?t=" + _pwd + "&p=%00";
		}
		else if (_camera >= HERO4)
		{
			request = _url + "command/shutter?p=0";
		}

		return sendHTTPRequest(request);
	}
	else // BLE
	{
#if defined(ARDUINO_ARCH_ESP32)
		sendBLERequest(BLE_RecordStop);
#endif
	}
}

////////////////////////////////////////////////////////////
////////                  Settings                  ////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setMode(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	if (WIFI_MODE)
	{
		String request;
		String parameter;

		if (_camera == HERO3)
		{
			switch (option)
			{
			case VIDEO_MODE:
				parameter = "00";
				break;
			case PHOTO_MODE:
				parameter = "01";
				break;
			case BURST_MODE:
				parameter = "02";
				break;
			case TIMELAPSE_MODE:
				parameter = "03";
				break;
			case TIMER_MODE:
				parameter = "04";
				break;
			case PLAY_HDMI_MODE:
				parameter = "05";
				break;
			default:
				_debug_port->println("Wrong parameter for setMode");
				return -1;
			}

			request = _url + "CM?t=" + _pwd + "&p=%" + parameter;
		}
		else if (_camera >= HERO4)
		{
			//todo: add sub-modes
			switch (option)
			{
			case VIDEO_MODE:
				parameter = "0";
				break;
			case PHOTO_MODE:
				parameter = "1";
				break;
			case MULTISHOT_MODE:
				parameter = "2";
				break;
			default:
				_debug_port->println("Wrong parameter for setMode");
				return -1;
			}

			request = _url + "command/mode?p=" + parameter;
		}

		return sendHTTPRequest(request);
	}
	else // BLE
	{
#if defined(ARDUINO_ARCH_ESP32)
		switch (option)
		{
		case VIDEO_MODE:
			sendBLERequest(BLE_ModeVideo);
			break;
		case PHOTO_MODE:
			sendBLERequest(BLE_ModePhoto);
			break;
		case MULTISHOT_MODE:
			sendBLERequest(BLE_ModeMultiShot);
			break;
		default:
			_debug_port->println("Wrong parameter for setMode");
			return -1;
		}
#endif
	}
}

uint8_t GoProControl::setOrientation(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (option)
		{
		case ORIENTATION_UP:
			parameter = "00";
			break;
		case ORIENTATION_DOWN:
			parameter = "01";
			break;
		default:
			_debug_port->println("Wrong parameter for setOrientation");
			return -1;
		}

		request = _url + "UP?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case ORIENTATION_UP:
			parameter = "0";
			break;
		case ORIENTATION_DOWN:
			parameter = "1";
			break;
		case ORIENTATION_AUTO:
			parameter = "2";
			break;
		default:
			_debug_port->println("Wrong parameter for setOrientation");
			return -1;
		}

		request = _url + "setting/52/" + parameter;
	}

	return sendHTTPRequest(request);
}

////////////////////////////////////////////////////////////
////////                   Video                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setVideoResolution(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (option)
		{
		case VR_1080p:
			parameter = "06";
			break;
		case VR_960p:
			parameter = "05";
			break;
		case VR_720p:
			parameter = "03";
			break;
		case VR_WVGA:
			parameter = "01";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoResolution");
			return -1;
		}

		request = _url + "VR?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case VR_4K:
			parameter = "1";
			break;
		case VR_2K:
			parameter = "4";
			break;
		case VR_2K_SuperView:
			parameter = "5";
			break;
		case VR_1440p:
			parameter = "7";
			break;
		case VR_1080p_SuperView:
			parameter = "8";
			break;
		case VR_1080p:
			parameter = "9";
			break;
		case VR_960p:
			parameter = "10";
			break;
		case VR_720p_SuperView:
			parameter = "11";
			break;
		case VR_720p:
			parameter = "12";
			break;
		case VR_WVGA:
			parameter = "13";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoResolution");
			return -1;
		}

		request = _url + "setting/2/" + parameter;
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::setVideoFov(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (option)
		{
		case WIDE_FOV:
			parameter = "00";
			break;
		case MEDIUM_FOV:
			parameter = "01";
			break;
		case NARROW_FOV:
			parameter = "02";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoFov");
			return -1;
		}

		request = _url + "FV?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case WIDE_FOV:
			parameter = "0";
			break;
		case MEDIUM_FOV:
			parameter = "1";
			break;
		case NARROW_FOV:
			parameter = "2";
			break;
		case LINEAR_FOV:
			parameter = "4";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoFov");
			return -1;
		}

		request = _url + "setting/4/" + parameter;
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::setFrameRate(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (option)
		{
		case FR_240:
			parameter = "0a";
			break;
		case FR_120:
			parameter = "09";
			break;
		case FR_100:
			parameter = "08";
			break;
		case FR_60:
			parameter = "07";
			break;
		case FR_50:
			parameter = "06";
			break;
		case FR_48:
			parameter = "05";
			break;
		case FR_30:
			parameter = "04";
			break;
		case FR_25:
			parameter = "03";
			break;
		case FR_24:
			parameter = "02";
			break;
		case FR_12p5:
			parameter = "0b";
			break;
		case FR_15:
			parameter = "01";
			break;
		case FR_12:
			parameter = "00";
			break;
		default:
			_debug_port->println("Wrong parameter for setFrameRate");
			return -1;
		}

		request = _url + "FS?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case FR_240:
			parameter = "0";
			break;
		case FR_120:
			parameter = "1";
			break;
		case FR_100:
			parameter = "2";
			break;
		case FR_90:
			parameter = "3";
			break;
		case FR_80:
			parameter = "4";
			break;
		case FR_60:
			parameter = "5";
			break;
		case FR_50:
			parameter = "6";
			break;
		case FR_48:
			parameter = "7";
			break;
		case FR_30:
			parameter = "8";
			break;
		case FR_25:
			parameter = "9";
			break;
		default:
			_debug_port->println("Wrong parameter for setFrameRate");
			return -1;
		}

		request = _url + "setting/3/" + parameter;
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::setVideoEncoding(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (option)
		{
		case NTSC:
			parameter = "00";
			break;
		case PAL:
			parameter = "01";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoEncoding");
			return -1;
		}

		request = _url + "VM?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case NTSC:
			parameter = "0";
			break;
		case PAL:
			parameter = "1";
			break;
		default:
			_debug_port->println("Wrong parameter for setVideoEncoding");
			return -1;
		}

		request = _url + "setting/57/" + parameter;
	}

	return sendHTTPRequest(request);
}

////////////////////////////////////////////////////////////
////////                   Photo                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setPhotoResolution(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (option)
		{
		case PR_11MP_WIDE:
			parameter = "00";
			break;
		case PR_8MP_WIDE:
			parameter = "01";
			break;
		case PR_5MP_WIDE:
			parameter = "02";
			break;
		default:
			_debug_port->println("Wrong parameter for setPhotoResolution");
			return -1;
		}

		request = _url + "PR?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		switch (option)
		{
		case PR_12MP_WIDE:
			parameter = "0";
			break;
		case PR_12MP_LINEAR:
			parameter = "10";
			break;
		case PR_12MP_MEDIUM:
			parameter = "8";
			break;
		case PR_12MP_NARROW:
			parameter = "9";
			break;
		case PR_7MP_WIDE:
			parameter = "1";
			break;
		case PR_7MP_MEDIUM:
			parameter = "2";
			break;
		case PR_5MP_WIDE:
			parameter = "3";
			break;
		default:
			_debug_port->println("Wrong parameter for setPhotoResolution");
			return -1;
		}

		request = _url + "setting/17/" + parameter;
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::setTimeLapseInterval(float option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	// convert float to integer
	if (option == 0.5)
	{
		option = 0;
	}
	const uint8_t i_option = (uint8_t)option;

	if (i_option != 0 || i_option != 1 || i_option != 5 || i_option != 10 || i_option != 30 || i_option != 60)
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setTimeLapseInterval");
		}
		return -1;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (i_option)
		{
		case 60:
			parameter = "3c";
			break;
		case 30:
			parameter = "1e";
			break;
		case 10:
			parameter = "0a";
			break;
		case 5:
			parameter = "05";
			break;
		case 1:
			parameter = "01";
			break;
		case 0: // 0.5
			parameter = "00";
			break;
		default:
			_debug_port->println("Wrong parameter for setTimeLapseInterval");
			return -1;
		}

		request = _url + "TI?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		switch (i_option)
		{
		case 60:
			parameter = "6";
			break;
		case 30:
			parameter = "5";
			break;
		case 10:
			parameter = "4";
			break;
		case 5:
			parameter = "3";
			break;
		case 1:
			parameter = "1";
			break;
		case 0: // 0.5
			parameter = "0";
			break;
		default:
			_debug_port->println("Wrong parameter for setTimeLapseInterval");
			return -1;
		}

		request = _url + "setting/5/" + parameter;
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::setContinuousShot(const uint8_t option)
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	// convert float to integer
	const uint8_t i_option = (uint8_t)option;

	if (i_option != 0 || i_option != 3 || i_option != 5 || i_option != 10)
	{
		if (_debug)
		{
			_debug_port->println("Wrong parameter for setContinuousShot");
		}
		return -1;
	}

	String request;
	String parameter;

	if (_camera == HERO3)
	{
		switch (option)
		{
		case 10:
			parameter = "0a";
			break;
		case 5:
			parameter = "05";
			break;
		case 3:
			parameter = "03";
			break;
		case 0:
			parameter = "00";
			break;
		default:
			_debug_port->println("Wrong parameter for setContinuousShot");
			return -1;
		}

		request = _url + "CS?t=" + _pwd + "&p=%" + parameter;
	}
	else if (_camera >= HERO4)
	{
		// Not supported in Hero4/5/6/7
		return false;
	}
	return sendHTTPRequest(request);
}

////////////////////////////////////////////////////////////
////////                   Others                   ////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::localizationOn()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;

	if (_camera == HERO3)
	{
		request = _url + "LL?t=" + _pwd + "&p=%01";
	}
	else if (_camera >= HERO4)
	{
		request = _url + "command/system/locate?p=1";
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::localizationOff()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}
	//todo test to turn it on and off with the camera off

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;

	if (_camera == HERO3)
	{
		request = _url + "LL?t=" + _pwd + "&p=%00";
	}
	else if (_camera >= HERO4)
	{
		request = _url + "command/system/locate?p=0";
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::deleteLast()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;

	if (_camera == HERO3)
	{
		request = _url + "DL?t=" + _pwd;
	}
	else if (_camera >= HERO4)
	{
		request = _url + "command/storage/delete/last";
	}

	return sendHTTPRequest(request);
}

uint8_t GoProControl::deleteAll()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	if (!isOn()) // camera is off
	{
		if (_debug)
		{
			_debug_port->println("turn on the camera first");
		}
		return false;
	}

	String request;

	if (_camera == HERO3)
	{
		request = _url + "DA?t=" + _pwd;
	}
	else if (_camera >= HERO4)
	{
		request = _url + "command/command/storage/delete/all";
	}

	return sendHTTPRequest(request);
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
	if (_debug)
	{
		_debug_port->print("\nSSID: ");
		_debug_port->println(WiFi.SSID());
		_debug_port->print("IP Address: ");
		_debug_port->println(WiFi.localIP());
		_debug_port->print("signal strength (RSSI):");
		_debug_port->print(WiFi.RSSI());
		_debug_port->println(" dBm\n");
		//WiFi.BSSID(bssid); WiFi.macAddress();
		//todo add more info like mode (photo, video), fow and so on
		// convert this output in a human readable https://github.com/KonradIT/goprowifihack/blob/master/HERO5/HERO5-Commands.md#gopro-hero5-commands-status-and-notes
	}
}

////////////////////////////////////////////////////////////
////////                  Private                  /////////
////////////////////////////////////////////////////////////

void GoProControl::sendWoL()
{
	uint8_t preamble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	IPAddress addr(255, 255, 255, 255);

	_udp_client.begin(_udp_port);
	_udp_client.beginPacket(addr, _udp_port);

	_udp_client.write(preamble, sizeof preamble);

	for (uint8_t i = 0; i < 16; i++)
	{
		_udp_client.write(_mac_address, 6);
	}
	_udp_client.endPacket();
	_udp_client.stop();
	delay(2000);
}

uint8_t GoProControl::sendRequest(const String request)
{
	if (!connectClient())
	{
		return false;
	}

	if (_debug)
	{
		_debug_port->println("Request: " + request);
	}
	_wifi_client.println(request);
	_wifi_client.stop();
}

uint8_t GoProControl::sendHTTPRequest(const String request)
{
	if (!connectClient())
	{
		return false;
	}

	_http.get(request);
	uint16_t response = _http.responseStatusCode();

	_http.stop();
	_wifi_client.stop();

	if (_debug)
	{
		_debug_port->println("HTTP request: " + request);
		_debug_port->print("Response: ");
		_debug_port->println(response);
	}

	if (response == 200)
	{
		if (_debug)
		{
			_debug_port->println("Command: Accepted");
		}
		return true;
	}
	else if (response == 403)
	{
		if (_debug)
		{
			_debug_port->println("Command: Wrong password");
		}
		return -1;
	}
	else if (response == 410)
	{
		if (_debug)
		{
			_debug_port->println("Command: Failed");
		}
		return -2;
	}
	else
	{
		if (_debug)
		{
			_debug_port->println("Command: Unknown error");
		}
		return -3;
	}
}

#if defined(ARDUINO_ARCH_ESP32)
uint8_t GoProControl::sendBLERequest(const uint8_t request[])
{
	_debug_port->println("BLE request:");
	for (uint8_t i = 0; i <= 3; i++)
	{
		_debug_port->println(request[i]);
	}
}
#endif

uint8_t GoProControl::connectClient()
{
	if (!_wifi_client.connect(_host.c_str(), _wifi_port))
	{
		if (_debug)
		{
			_debug_port->println("Connection lost");
		}
		_connected = false;
		return false;
	}
	else
	{
		if (_debug)
		{
			_debug_port->println("Client connected");
		}
		_last_request = millis();
		return true;
	}
}

uint8_t GoProControl::confirmPairing()
{
	if (!checkConnection()) // not connected
	{
		if (_debug)
		{
			_debug_port->println("Connect the camera first");
		}
		return false;
	}

	String request;

	if (_camera == HERO3)
	{
		if (_debug)
		{
			_debug_port->println("Not supported");
		}
		return false;
	}
	else if (_camera == HERO4)
	{
		// https://github.com/KonradIT/goprowifihack/blob/master/HERO4/WifiCommands.md#code-pairing
		// todo
	}
	else if (_camera >= HERO5)
	{
		request = _url + "command/wireless/pair/complete?success=1&deviceName=" + _board_name;
	}

	return sendHTTPRequest(request);
}
