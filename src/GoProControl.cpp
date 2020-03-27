/*
GoProControl.cpp

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <GoProControl.h>
#include <Utilities.h>

// todo _request as char array

////////////////////////////////////////////////////////////
////////                Constructor                 ////////
////////////////////////////////////////////////////////////

GoProControl::GoProControl(const char *ssid, const char *pwd, const uint8_t camera,
                           const uint8_t gopro_mac[], const char *board_name)
{
  _ssid = (char *)ssid;
  _pwd = (char *)pwd;
  _camera = camera;

  // GoPro MAC
  if (gopro_mac != NULL)
  {
    memcpy(_gopro_mac, gopro_mac, MAC_ADDRESS_LENGTH);
  }
  else
  {
    memset(_gopro_mac, 0, MAC_ADDRESS_LENGTH); // Empty the array
  }

  // Board Hostname
#if defined(ARDUINO_ARCH_ESP32)
  if (strcmp(board_name, "") != 0)
  {
    WiFi.setHostname(board_name);
  }
#endif
  _board_name = (char *)"";

  memset(_board_mac, 0, MAC_ADDRESS_LENGTH); // Empty the array
}

////////////////////////////////////////////////////////////
////////               Communication               /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::begin()
{
  if (_connected == true)
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
    _debug_port->print("Attempting to connect to SSID: \"");
    _debug_port->print(_ssid);
    _debug_port->print("\"\nUsing password: \"");
    _debug_port->print(_pwd);
    _debug_port->print("\"\n");
  }

  WiFi.begin(_ssid, _pwd);

  uint32_t start_time = millis();
  while (WiFi.status() != WL_CONNECTED && start_time + MAX_WAIT_TIME > millis())
  {
    if (_debug)
    {
      _debug_port->print(".");
    }
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    if (_debug)
    {
      _debug_port->println("\nConnected to GoPro");
    }
    _connected = true;
    getWiFiData();
    return true;
  }
  else
  {
    if (_debug)
    {
      _debug_port->print("\nConnection failed with status: ");
      _debug_port->println(WiFi.status());
    }
    _connected = false;
  }

  return -(WiFi.status());
}

void GoProControl::end()
{
  if (_connected == false)
  {
    if (_debug)
    {
      _debug_port->println("Camera not connected yet");
    }
    return;
  }

  if (_debug)
  {
    _debug_port->println("Closing connection");
  }
  _udp_client.stop();
  _wifi_client.stop();
  WiFi.disconnect();
  _connected = false;
  _recording = false;
  memset(_gopro_mac, 0, MAC_ADDRESS_LENGTH);
}

uint8_t GoProControl::keepAlive()
{
  if (_connected == false) // camera not connected
  {
    return false;
  }

  if (millis() - _last_request <= KEEP_ALIVE)
  {
    // we made a request not so much earlier
    return false;
  }
  else
  { // time to ask something to the camera
    if (_camera == HERO3)
    {
      // not needed since the connection won't be closed by the camera (tested
      // for more then 6 minutes)
      return true;
    }
    else if (_camera >= HERO4)
    {
      if (_debug)
      {
        _debug_port->println("Keeping connection alive");
      }
      sendRequest("_GPHD_:0:0:2:0.000000\n");
    }
  }
  return true;
}

uint8_t GoProControl::confirmPairing()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    if (_debug)
    {
      _debug_port->println("Not supported by HERO3");
    }
    return false;
  }
  else if (_camera == HERO4)
  {
    if (_debug)
    {
      _debug_port->println("Not implemented yet, see readME");
    }
    return false;
  }
  else if (_camera >= HERO5)
  {
    makeRequest(_request, "/gp/gpControl/command/wireless/pair/complete?success=1&deviceName=",
                _board_name);
  }

  return handleHTTPRequest(_request);
}

////////////////////////////////////////////////////////////
////////                    BLE                    /////////
////////////////////////////////////////////////////////////

#if defined(ARDUINO_ARCH_ESP32)
// none of these function will work, I am adding these for a future release, see
// the readME
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
  return true;
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
  return true;
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

  if (BLE_ENABLED == false) // prevent stupid error like turn off the wifi
                            // while we didn't connected to the BL yet
  {
    if (_debug)
    {
      _debug_port->println("First run enableBLE()");
    }
    return false;
  }

  WIFI_MODE = false;
  return sendBLERequest(BLE_WiFiOff);
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
  return sendBLERequest(BLE_WiFiOn);
}
#endif // BLE functions

////////////////////////////////////////////////////////////
////////                  Control                  /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::turnOn()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    makeRequest(_request, "/bacpac/PW?t=", _pwd, "&p=%01");
  }
  else if (_camera >= HERO4)
  {
    if (_gopro_mac[0] == 0)
    {
      if (_debug)
      {
        _debug_port->println("No BSSID, unable to turn on the camera");
#if defined(ARDUINO_ARCH_ESP8266)
        _debug_port->println("The ESP8266 can't get the BSSID, you need to pass it in the "
                             "constructor, see the README");
#endif
      }
      return false;
    }
    else
    {
      sendWoL();
      if (isOn())
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::turnOff(const bool force)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    makeRequest(_request, "/bacpac/PW?t=", _pwd, "&p=%00");
  }
  else if (_camera >= HERO4)
  {
    if (_gopro_mac[0] == 0 && force == false)
    {
      getBSSID();
      if (_debug)
      {
        _debug_port->println("BSSID not ready, try again");
      }
      return false;
    }
    else if (_gopro_mac[0] == 0 && force)
    {
      if (_debug)
      {
        _debug_port->println(
            "Forcing turnOff, you won't be able to turnOn again from arduino");
      }
    }
    makeRequest(_request, "/gp/gpControl/command/system/sleep");
  }

  return handleHTTPRequest(_request);
}

////////////////////////////////////////////////////////////
//////                     Status                     //////
////////////////////////////////////////////////////////////

char *GoProControl::getStatus()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return '\0';
  }

  if (_camera == HERO3)
  {
    makeRequest(_request, "/camera/sx?t=", _pwd);
  }
  else if (_camera >= HERO4)
  {
    makeRequest(_request, "/gp/gpControl/status");
  }

  sendHTTPRequest(_request);
  if (listenResponse())
  {
    int16_t start = stringSearch(_response_buffer, "{\"s");
    int16_t end = stringSearch(_response_buffer, "}}") + 2;
    if (start != -1 && end != -1)
    {
      return stringCut(_response_buffer, start, end);
    }
  }
  return '\0';
}

char *GoProControl::getMediaList()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return '\0';
  }

  sendHTTPRequest("/gp/gpMediaList");
  if (listenResponse())
  {
    int16_t start = stringSearch(_response_buffer, "{\"i");
    int16_t end = stringSearch(_response_buffer, "}]}]}") + 5; // 5 is the length of the pattern
    if (start != -1 && end != -1)
    {
      return stringCut(_response_buffer, start, end);
    }
  }
  return '\0';
}

bool GoProControl::isOn()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    // this isn't supported by this camera so this function will always return
    // true
    return true;
  }
  else if (_camera >= HERO4)
  {
    makeRequest(_request, "/gp/gpControl/status");
  }

  if (sendHTTPRequest(_request) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool GoProControl::isConnected(const bool silent)
{
  if (_connected == true)
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

bool GoProControl::isRecording()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  return _recording;
}

////////////////////////////////////////////////////////////
////////                   Shoot                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::shoot()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  bool result = false;

  if (WIFI_MODE)
  {
    if (_camera == HERO3)
    {
      makeRequest(_request, "/bacpac/SH?t=", _pwd, "&p=%01");
    }
    else if (_camera >= HERO4)
    {
      makeRequest(_request, "/gp/gpControl/command/shutter?p=1");
    }

    result = handleHTTPRequest(_request);
  }
  else
  { // BLE
#if defined(ARDUINO_ARCH_ESP32)
    result = sendBLERequest(BLE_RecordStart);
#endif
  }
  if (result == true && _mode >= VIDEO_MODE && _mode <= VIDEO_TIMEWARP_MODE)
  {
    _recording = true;
  }
  return result;
}

uint8_t GoProControl::stopShoot()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  bool result = false;

  if (WIFI_MODE)
  {
    if (_camera == HERO3)
    {
      makeRequest(_request, "/bacpac/SH?t=", _pwd, "&p=%00");
    }
    else if (_camera >= HERO4)
    {
      makeRequest(_request, "/gp/gpControl/command/shutter?p=0");
    }

    result = handleHTTPRequest(_request);
  }
  else // BLE
  {
#if defined(ARDUINO_ARCH_ESP32)
    result = sendBLERequest(BLE_RecordStop);
#endif
  }
  if (result == true)
  {
    _recording = false;
  }
  return result;
}

////////////////////////////////////////////////////////////
////////                  Settings                  ////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setMode(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  bool result = false;

  if (WIFI_MODE)
  {
    if (_camera == HERO3)
    {
      switch (option)
      {
      case VIDEO_MODE:
        _parameter = (char *)"00";
        break;
      case PHOTO_MODE:
        _parameter = (char *)"01";
        break;
      case BURST_MODE:
        _parameter = (char *)"02";
        break;
      case TIMELAPSE_MODE:
        _parameter = (char *)"03";
        break;
      case TIMER_MODE:
        _parameter = (char *)"04";
        break;
      case PLAY_HDMI_MODE:
        _parameter = (char *)"05";
        break;
      default:
        if (_debug)
        {
          _debug_port->println("Wrong parameter for setMode");
        }
        return -1;
      }
      makeRequest(_request, "camera/CM?t=", _pwd, "&p=%", _parameter);
    }
    else if (_camera >= HERO4)
    {
      _sub_parameter = (char *)"";
      switch (option)
      {
      case VIDEO_MODE:
        _parameter = (char *)"0";
        break;
      case VIDEO_SUB_MODE:
        _parameter = (char *)"0";
        _sub_parameter = (char *)"0";
        break;
      case VIDEO_TIMELAPSE_MODE:
        _parameter = (char *)"0";
        _sub_parameter = (char *)"1";
        break;
      case VIDEO_PHOTO_MODE:
        _parameter = (char *)"0";
        _sub_parameter = (char *)"2";
        break;
      case VIDEO_LOOPING_MODE:
        _parameter = (char *)"0";
        _sub_parameter = (char *)"3";
        break;
      case VIDEO_TIMEWARP_MODE:
        _parameter = (char *)"0";
        _sub_parameter = (char *)"4";
        break;

      case PHOTO_MODE:
        _parameter = (char *)"1";
        break;
      case PHOTO_SINGLE_MODE:
        _parameter = (char *)"1";
        _sub_parameter = (char *)"1";
        break;
      case PHOTO_NIGHT_MODE:
        _parameter = (char *)"1";
        _sub_parameter = (char *)"2";
        break;

      case MULTISHOT_MODE:
        _parameter = (char *)"2";
        break;
      case MULTISHOT_BURST_MODE:
        _parameter = (char *)"2";
        _sub_parameter = (char *)"0";
        break;
      case MULTISHOT_TIMELAPSE_MODE:
        _parameter = (char *)"2";
        _sub_parameter = (char *)"1";
        break;
      case MULTISHOT_NIGHTLAPSE_MODE:
        _parameter = (char *)"2";
        _sub_parameter = (char *)"2";
        break;

      default:
        if (_debug)
        {
          _debug_port->println("Wrong parameter for setMode");
        }
        return -1;
      }

      if (_sub_parameter == (char *)"")
      {
        makeRequest(_request, "/gp/gpControl/command/mode?p=", _parameter);
      }
      else
      {
        makeRequest(_request, "/gp/gpControl/command/sub_mode?mode=", _parameter,
                    "&sub_mode=", _sub_parameter);
      }
    }
    _mode = option;
    result = handleHTTPRequest(_request);
  }
  else
  { // BLE
#if defined(ARDUINO_ARCH_ESP32)
    switch (option)
    {
    case VIDEO_MODE:
      result = sendBLERequest(BLE_ModeVideo);
      break;
    case PHOTO_MODE:
      result = sendBLERequest(BLE_ModePhoto);
      break;
    case MULTISHOT_MODE:
      result = sendBLERequest(BLE_ModeMultiShot);
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setMode");
      }
      return -1;
    }
#endif
  }
  return result;
}

uint8_t GoProControl::setOrientation(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    switch (option)
    {
    case ORIENTATION_UP:
      _parameter = (char *)"00";
      break;
    case ORIENTATION_DOWN:
      _parameter = (char *)"01";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setOrientation");
      }
      return -1;
    }
    makeRequest(_request, "camera/UP?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4)
  {
    switch (option)
    {
    case ORIENTATION_UP:
      _parameter = (char *)"0";
      break;
    case ORIENTATION_DOWN:
      _parameter = (char *)"1";
      break;
    case ORIENTATION_AUTO:
      _parameter = (char *)"2";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setOrientation");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/52/", _parameter);
  }

  return handleHTTPRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Video                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setVideoResolution(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    switch (option)
    {
    case VR_1080p:
      _parameter = (char *)"06";
      break;
    case VR_960p:
      _parameter = (char *)"05";
      break;
    case VR_720p:
      _parameter = (char *)"03";
      break;
    case VR_WVGA:
      _parameter = (char *)"01";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setVideoResolution");
      }
      return -1;
    }
    makeRequest(_request, "camera/VR?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4)
  {
    switch (option)
    {
    case VR_5p6K:
      _parameter = (char *)"21";
      break;
    case VR_4K:
      _parameter = (char *)"1";
      break;
    case VR_2K:
      _parameter = (char *)"4";
      break;
    case VR_2K_SuperView:
      _parameter = (char *)"5";
      break;
    case VR_1440p:
      _parameter = (char *)"7";
      break;
    case VR_1080p_SuperView:
      _parameter = (char *)"8";
      break;
    case VR_1080p:
      _parameter = (char *)"9";
      break;
    case VR_960p:
      _parameter = (char *)"10";
      break;
    case VR_720p_SuperView:
      _parameter = (char *)"11";
      break;
    case VR_720p:
      _parameter = (char *)"12";
      break;
    case VR_WVGA:
      _parameter = (char *)"13";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setVideoResolution");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/2/", _parameter);
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::setVideoFov(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    switch (option)
    {
    case WIDE_FOV:
      _parameter = (char *)"00";
      break;
    case MEDIUM_FOV:
      _parameter = (char *)"01";
      break;
    case NARROW_FOV:
      _parameter = (char *)"02";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setVideoFov");
      }
      return -1;
    }

    makeRequest(_request, "camera/FV?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4 && _camera <= HERO7)
  {
    switch (option)
    {
    case WIDE_FOV:
      _parameter = (char *)"0";
      break;
    case MEDIUM_FOV:
      _parameter = (char *)"1";
      break;
    case NARROW_FOV:
      _parameter = (char *)"2";
      break;
    case LINEAR_FOV:
      _parameter = (char *)"4";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setVideoFov");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/4/", _parameter);
  }
  else if (_camera >= HERO8)
  {
    switch (option)
    {
    case DUAL360_FOV:
      _parameter = (char *)"5";
      break;
    case WIDE_FOV:
      _parameter = (char *)"0";
      break;
    case MEDIUM_FOV:
      _parameter = (char *)"1";
      break;
    case NARROW_FOV:
      _parameter = (char *)"6";
      break;
    case LINEAR_FOV:
      _parameter = (char *)"4";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setVideoFov");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/121/", _parameter);
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::setFrameRate(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    switch (option)
    {
    case FR_240:
      _parameter = (char *)"0a";
      break;
    case FR_120:
      _parameter = (char *)"09";
      break;
    case FR_100:
      _parameter = (char *)"08";
      break;
    case FR_60:
      _parameter = (char *)"07";
      break;
    case FR_50:
      _parameter = (char *)"06";
      break;
    case FR_48:
      _parameter = (char *)"05";
      break;
    case FR_30:
      _parameter = (char *)"04";
      break;
    case FR_25:
      _parameter = (char *)"03";
      break;
    case FR_24:
      _parameter = (char *)"02";
      break;
    case FR_12p5:
      _parameter = (char *)"0b";
      break;
    case FR_15:
      _parameter = (char *)"01";
      break;
    case FR_12:
      _parameter = (char *)"00";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setFrameRate");
      }
      return -1;
    }
    makeRequest(_request, "camera/FS?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4)
  {
    switch (option)
    {
    case FR_240:
      _parameter = (char *)"0";
      break;
    case FR_120:
      _parameter = (char *)"1";
      break;
    case FR_100:
      _parameter = (char *)"2";
      break;
    case FR_90:
      _parameter = (char *)"3";
      break;
    case FR_80:
      _parameter = (char *)"4";
      break;
    case FR_60:
      _parameter = (char *)"5";
      break;
    case FR_50:
      _parameter = (char *)"6";
      break;
    case FR_48:
      _parameter = (char *)"7";
      break;
    case FR_30:
      _parameter = (char *)"8";
      break;
    case FR_25:
      _parameter = (char *)"9";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setFrameRate");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/3/", _parameter);
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::setVideoEncoding(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    switch (option)
    {
    case NTSC:
      _parameter = (char *)"00";
      break;
    case PAL:
      _parameter = (char *)"01";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setVideoEncoding");
      }
      return -1;
    }
    makeRequest(_request, "camera/VM?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4)
  {
    switch (option)
    {
    case NTSC:
      _parameter = (char *)"0";
      break;
    case PAL:
      _parameter = (char *)"1";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setVideoEncoding");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/57/", _parameter);
  }

  return handleHTTPRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Photo                   /////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::setPhotoResolution(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    switch (option)
    {
    case PR_11MP_WIDE:
      _parameter = (char *)"00";
      break;
    case PR_8MP_WIDE:
      _parameter = (char *)"01";
      break;
    case PR_5MP_WIDE:
      _parameter = (char *)"02";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setPhotoResolution");
      }
      return -1;
    }
    makeRequest(_request, "camera/PR?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4)
  {
    switch (option)
    {
    case PR_12MP_WIDE:
      _parameter = (char *)"0";
      break;
    case PR_12MP_LINEAR:
      _parameter = (char *)"10";
      break;
    case PR_12MP_MEDIUM:
      _parameter = (char *)"8";
      break;
    case PR_12MP_NARROW:
      _parameter = (char *)"9";
      break;
    case PR_7MP_WIDE:
      _parameter = (char *)"1";
      break;
    case PR_7MP_MEDIUM:
      _parameter = (char *)"2";
      break;
    case PR_5MP_WIDE:
      _parameter = (char *)"3";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setPhotoResolution");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/17/", _parameter);
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::setTimeLapseInterval(float option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  // convert float to integer
  if (option == 0.5)
  {
    option = 0;
  }
  const uint8_t i_option = (uint8_t)option;

  if (i_option != 0 && i_option != 1 && i_option != 5 && i_option != 10 && i_option != 30 &&
      i_option != 60)
  {
    if (_debug)
    {
      _debug_port->println("Wrong parameter for setTimeLapseInterval");
    }
    return -1;
  }

  if (_camera == HERO3)
  {
    switch (i_option)
    {
    case 60:
      _parameter = (char *)"3c";
      break;
    case 30:
      _parameter = (char *)"1e";
      break;
    case 10:
      _parameter = (char *)"0a";
      break;
    case 5:
      _parameter = (char *)"05";
      break;
    case 1:
      _parameter = (char *)"01";
      break;
    case 0: // 0.5
      _parameter = (char *)"00";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setTimeLapseInterval");
      }
      return -1;
    }
    makeRequest(_request, "camera/TI?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4)
  {
    switch (i_option)
    {
    case 60:
      _parameter = (char *)"6";
      break;
    case 30:
      _parameter = (char *)"5";
      break;
    case 10:
      _parameter = (char *)"4";
      break;
    case 5:
      _parameter = (char *)"3";
      break;
    case 1:
      _parameter = (char *)"1";
      break;
    case 0: // 0.5
      _parameter = (char *)"0";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setTimeLapseInterval");
      }
      return -1;
    }
    makeRequest(_request, "/gp/gpControl/setting/5/", _parameter);
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::setContinuousShot(const uint8_t option)
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
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

  if (_camera == HERO3)
  {
    switch (option)
    {
    case 10:
      _parameter = (char *)"0a";
      break;
    case 5:
      _parameter = (char *)"05";
      break;
    case 3:
      _parameter = (char *)"03";
      break;
    case 0:
      _parameter = (char *)"00";
      break;
    default:
      if (_debug)
      {
        _debug_port->println("Wrong parameter for setContinuousShot");
      }
      return -1;
    }
    makeRequest(_request, "camera/CS?t=", _pwd, "&p=%", _parameter);
  }
  else if (_camera >= HERO4)
  {
    if (_debug)
    {
      _debug_port->println("Not supported by HERO4 and newer");
    }
    return false;
  }
  return handleHTTPRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Others                   ////////
////////////////////////////////////////////////////////////

uint8_t GoProControl::localizationOn()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    makeRequest(_request, "camera/LL?t=", _pwd, "&p=%01");
  }
  else if (_camera >= HERO4)
  {
    makeRequest(_request, "/gp/gpControl/command/system/locate?p=1");
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::localizationOff()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    makeRequest(_request, "camera/LL?t=", _pwd, "&p=%00");
  }
  else if (_camera >= HERO4)
  {
    makeRequest(_request, "/gp/gpControl/command/system/locate?p=0");
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::deleteLast()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    makeRequest(_request, "camera/DL?t=", _pwd);
  }
  else if (_camera >= HERO4)
  {
    makeRequest(_request, "/gp/gpControl/command/storage/delete/last");
  }

  return handleHTTPRequest(_request);
}

uint8_t GoProControl::deleteAll()
{
  if (_connected == false) // not connected
  {
    if (_debug)
    {
      _debug_port->println("Connect the camera first");
    }
    return false;
  }

  if (_camera == HERO3)
  {
    makeRequest(_request, "camera/DA?t=", _pwd);
  }
  else if (_camera >= HERO4)
  {
    makeRequest(_request, "/gp/gpControl/command/storage/delete/all");
  }

  return handleHTTPRequest(_request);
}

////////////////////////////////////////////////////////////
////////                   Debug                   /////////
////////////////////////////////////////////////////////////

void GoProControl::enableDebug(UniversalSerial *debug_port, const uint32_t debug_baudrate)
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

void GoProControl::printStatus()
{
  if (_debug)
  {
    _debug_port->print("\nSSID:\t\t");
    _debug_port->println(_ssid);
    _debug_port->print("Password:\t");
    _debug_port->println(_pwd);
    _debug_port->print("Camera:\t\t");
    if (_camera == 8)
    {
      _debug_port->println("FUSION");
    }
    else if (_camera == 10)
    {
      _debug_port->println("MAX");
    }
    else
    {
      _debug_port->print("HERO");
      if (_camera <= 7)
      {
        _debug_port->println(_camera);
      }
      else
      {
        _debug_port->println(_camera - 1);
      }
    }
    _debug_port->print("Board Name:\t");
    _debug_port->println(_board_name);
    _debug_port->print("IP Address:\t");
    _debug_port->println(WiFi.localIP());
    _debug_port->print("RSSI:\t\t");
    _debug_port->print(WiFi.RSSI());
    _debug_port->println(" dBm");
    _debug_port->print("Board MAC:\t");
    printArray(_board_mac, MAC_ADDRESS_LENGTH, ":", HEX, false, false, _debug_port);
    _debug_port->print("GoPro MAC:\t");
    printArray(_gopro_mac, MAC_ADDRESS_LENGTH, ":", HEX, false, false, _debug_port);
    _debug_port->println();
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

  _udp_client.write(preamble, LEN(preamble));

  for (uint8_t i = 0; i < 16; i++)
  {
    _udp_client.write(_gopro_mac, MAC_ADDRESS_LENGTH);
  }
  _udp_client.endPacket();
  _udp_client.stop();
}

uint8_t GoProControl::sendRequest(const char *request, bool silent)
{
  if (!connectClient())
  {
    return false;
  }

  if (_debug && silent == false)
  {
    _debug_port->print("Request: ");
    _debug_port->println(request);
  }
  _wifi_client.println(request);
  _wifi_client.stop();
  return true;
}

bool GoProControl::handleHTTPRequest(const char *request)
{
  if (sendHTTPRequest(request))
  {
    listenResponse();
    if (extractResponseCode() == 200)
    {
      return true;
    }
  }
  return false;
}

bool GoProControl::sendHTTPRequest(const char *request)
{
  if (!connectClient())
  {
    return false;
  }

  if (_debug)
  {
    _debug_port->print("HTTP request: ");
    _debug_port->println(request);
  }
  _wifi_client.print("GET ");
  _wifi_client.print(request);
  _wifi_client.println(" HTTP/1.1");
  if (_camera == HERO3)
  {
    _wifi_client.print("Host: ");
    _wifi_client.print(_host);
    _wifi_client.print(":");
    _wifi_client.println(_wifi_port);
  }
  else if (_camera >= HERO4)
  {
    _wifi_client.print("Host: ");
    _wifi_client.println(_host);
  }
  _wifi_client.println("Connection: close");
  _wifi_client.println();
  return true;
}

#if defined(ARDUINO_ARCH_ESP32)
uint8_t GoProControl::sendBLERequest(const uint8_t request[])
{
  if (_debug)
  {
    _debug_port->println("BLE request:");
    for (uint8_t i = 0; i <= 3; i++)
    {
      _debug_port->println(request[i]);
    }
  }
  return true;
}

#endif

uint8_t GoProControl::connectClient()
{
  if (!_wifi_client.connect(_host, _wifi_port))
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

bool GoProControl::listenResponse()
{
  uint16_t index = 0;

  if (_debug)
  {
    _debug_port->print("Waiting response");
  }
  uint32_t start_time = millis();
  while ((_wifi_client.available() == 0) && (start_time + MAX_WAIT_TIME > millis()))
  {
    if (_debug)
    {
      _debug_port->print(".");
      delay(5);
    }
  }
  while (_wifi_client.available() > 0)
  {
    _response_buffer[index++] = _wifi_client.read();
  }
  _response_buffer[index] = '\0';
  _wifi_client.stop();

  if (_debug)
  {
    _debug_port->println("\nStart response body");
    _debug_port->print(_response_buffer);
    _debug_port->println("\nEnd response body");
  }
  if (strcmp(_response_buffer, "") == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

uint16_t GoProControl::extractResponseCode()
{
  if (strcmp(_response_buffer, "") == 0)
  {
    return false;
  }

  int8_t start = stringSearch(_response_buffer, " ") + 1;
  int8_t end = stringSearch(_response_buffer, " ", start);
  uint16_t code = atoi(stringCut(_response_buffer, start, end));

  if (_debug)
  {
    _debug_port->print("Response code: ");
    _debug_port->println(code);
    if (code == 200)
    {
      _debug_port->println("Command: Accepted");
    }
    else if (code == 400)
    {
      _debug_port->println("Command: Bad request");
    }
    else if (code == 403)
    {
      _debug_port->println("Command: Wrong password");
    }
    else if (code == 410)
    {
      _debug_port->println("Command: Failed");
    }
    else
    {
      _debug_port->println("Command: Other error");
    }
  }

  return code;
}

void GoProControl::getBSSID()
{
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
  // ESP32 and ESP8266 aren't compliant with the arduino API
  _gopro_mac = WiFi.BSSID();
#else
  WiFi.BSSID(_gopro_mac);
#endif

#if not defined(INVERT_MAC) // Invert the mac in some boards
  revert(_gopro_mac);
#endif
}

void GoProControl::getWiFiData()
{
  if (_gopro_mac[0] == 0)
  {
    getBSSID();
  }
  if (_board_mac[0] == 0)
  {
    WiFi.macAddress(_board_mac);
#if not defined(INVERT_MAC) // Invert the mac in arduino boards
    revert(_board_mac);
#endif
  }
  if (strcmp(_board_name, "") == 0)
  {
#if defined(ARDUINO_ARCH_ESP32)
    _board_name = (char *)WiFi.getHostname();
#else
    // not supported by arduino api
#endif
  }
}

void GoProControl::revert(uint8_t mac[])
{
  uint8_t temp[MAC_ADDRESS_LENGTH];
  for (uint8_t i = 0; i < MAC_ADDRESS_LENGTH / 2; i++)
  {
    temp[i] = mac[i];
    mac[i] = mac[MAC_ADDRESS_LENGTH - i - 1];
    mac[MAC_ADDRESS_LENGTH - i - 1] = temp[i];
  }
}

void GoProControl::makeRequest(char *buff, const char *a, const char *b, const char *c,
                               const char *d)
{
  strcpy(buff, a);
  if (b != nullptr)
  {
    strcat(buff, b);
  }
  if (c != nullptr)
  {
    strcat(buff, c);
  }
  if (d != nullptr)
  {
    strcat(buff, d);
  }
}
