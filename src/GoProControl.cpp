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
#define LEN(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

////////////////////////////////////////////////////////////
////////                Constructors                ////////
////////////////////////////////////////////////////////////

GoProControl::GoProControl(const String ssid, const String pwd, const uint8_t camera)
{
    _ssid = ssid;
    _pwd = pwd;
    _camera = camera;
}

GoProControl::GoProControl(const String ssid, const String pwd, const uint8_t camera, const uint8_t mac_address[], const String board_name)
    : GoProControl(ssid, pwd, camera)
{
    memcpy(_mac_address, mac_address, LEN(_mac_address));
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

    WiFi.begin(_ssid.c_str(), _pwd.c_str());

    uint32_t start_time = millis();
    while (WiFi.status() != WL_CONNECTED && start_time + MAX_WAIT_TIME > millis())
    {
        if (_debug)
        {
            delay(100);
            _debug_port->print(".");
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        if (_debug)
        {
            _debug_port->println("\nConnected to GoPro");
        }
        _connected = true;
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
    if (!checkConnection())
    {
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
            // not needed since the connection won't be closed by the camera (tested for more then 6 minutes)
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
}

////////////////////////////////////////////////////////////
////////                    BLE                    /////////
////////////////////////////////////////////////////////////

#if defined(ARDUINO_ARCH_ESP32)
// none of these function will work, I am adding these for a future release, see the readME
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

    if (_camera == HERO3)
    {
        _request = "/bacpac/PW?t=" + _pwd + "&p=%01";
    }
    else if (_camera >= HERO4)
    {
        sendWoL();
        return true;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        _request = "/bacpac/PW?t=" + _pwd + "&p=%00";
    }
    else if (_camera >= HERO4)
    {
        _request = "/gp/gpControl/command/system/sleep";
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        // this isn't supported by this camera so this function will always return true
        return true;
    }
    else if (_camera >= HERO4)
    {
        _request = "/gp/gpControl/status";
    }

    return sendHTTPRequest(_request);
}

uint8_t GoProControl::checkConnection(const bool silent)
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

        if (_camera == HERO3)
        {
            _request = "/camera/SH?t=" + _pwd + "&p=%01";
        }
        else if (_camera >= HERO4)
        {
            _request = "/gp/gpControl/command/shutter?p=1";
        }

        return sendHTTPRequest(_request);
    }
    else // BLE
    {
#if defined(ARDUINO_ARCH_ESP32)
        sendBLERequest(BLE_RecordStart);
#else
        if (_debug)
        {
            _debug_port->println("This shouldn't be run");
        }
        return -1;
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

        if (_camera == HERO3)
        {
            _request = "/camera/SH?t=" + _pwd + "&p=%00";
        }
        else if (_camera >= HERO4)
        {
            _request = "/gp/gpControl/command/shutter?p=0";
        }

        return sendHTTPRequest(_request);
    }
    else // BLE
    {
#if defined(ARDUINO_ARCH_ESP32)
        sendBLERequest(BLE_RecordStop);
#else
        if (_debug)
        {
            _debug_port->println("This shouldn't be run");
        }
        return -1;
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

        if (_camera == HERO3)
        {
            switch (option)
            {
            case VIDEO_MODE:
                _parameter = "00";
                break;
            case PHOTO_MODE:
                _parameter = "01";
                break;
            case BURST_MODE:
                _parameter = "02";
                break;
            case TIMELAPSE_MODE:
                _parameter = "03";
                break;
            case TIMER_MODE:
                _parameter = "04";
                break;
            case PLAY_HDMI_MODE:
                _parameter = "05";
                break;
            default:
                _debug_port->println("Wrong parameter for setMode");
                return -1;
            }

            _request = "camera/CM?t=" + _pwd + "&p=%" + _parameter;
        }
        else if (_camera >= HERO4)
        {
            switch (option)
            {
            case VIDEO_MODE:
                _parameter = "0";
                break;
            case PHOTO_MODE:
                _parameter = "1";
                break;
            case MULTISHOT_MODE:
                _parameter = "2";
                break;
            default:
                _debug_port->println("Wrong parameter for setMode");
                return -1;
            }

            _request = "/gp/gpControl/command/mode?p=" + _parameter;
        }

        return sendHTTPRequest(_request);
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
#else
        if (_debug)
        {
            _debug_port->println("This shouldn't be run");
        }
        return -1;
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

    if (_camera == HERO3)
    {
        switch (option)
        {
        case ORIENTATION_UP:
            _parameter = "00";
            break;
        case ORIENTATION_DOWN:
            _parameter = "01";
            break;
        default:
            _debug_port->println("Wrong parameter for setOrientation");
            return -1;
        }

        _request = "camera/UP?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        switch (option)
        {
        case ORIENTATION_UP:
            _parameter = "0";
            break;
        case ORIENTATION_DOWN:
            _parameter = "1";
            break;
        case ORIENTATION_AUTO:
            _parameter = "2";
            break;
        default:
            _debug_port->println("Wrong parameter for setOrientation");
            return -1;
        }

        _request = "/gp/gpControl/setting/52/" + _parameter;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        switch (option)
        {
        case VR_1080p:
            _parameter = "06";
            break;
        case VR_960p:
            _parameter = "05";
            break;
        case VR_720p:
            _parameter = "03";
            break;
        case VR_WVGA:
            _parameter = "01";
            break;
        default:
            _debug_port->println("Wrong parameter for setVideoResolution");
            return -1;
        }

        _request = "camera/VR?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        switch (option)
        {
        case VR_4K:
            _parameter = "1";
            break;
        case VR_2K:
            _parameter = "4";
            break;
        case VR_2K_SuperView:
            _parameter = "5";
            break;
        case VR_1440p:
            _parameter = "7";
            break;
        case VR_1080p_SuperView:
            _parameter = "8";
            break;
        case VR_1080p:
            _parameter = "9";
            break;
        case VR_960p:
            _parameter = "10";
            break;
        case VR_720p_SuperView:
            _parameter = "11";
            break;
        case VR_720p:
            _parameter = "12";
            break;
        case VR_WVGA:
            _parameter = "13";
            break;
        default:
            _debug_port->println("Wrong parameter for setVideoResolution");
            return -1;
        }

        _request = "/gp/gpControl/setting/2/" + _parameter;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        switch (option)
        {
        case WIDE_FOV:
            _parameter = "00";
            break;
        case MEDIUM_FOV:
            _parameter = "01";
            break;
        case NARROW_FOV:
            _parameter = "02";
            break;
        default:
            _debug_port->println("Wrong parameter for setVideoFov");
            return -1;
        }

        _request = "camera/FV?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        switch (option)
        {
        case WIDE_FOV:
            _parameter = "0";
            break;
        case MEDIUM_FOV:
            _parameter = "1";
            break;
        case NARROW_FOV:
            _parameter = "2";
            break;
        case LINEAR_FOV:
            _parameter = "4";
            break;
        default:
            _debug_port->println("Wrong parameter for setVideoFov");
            return -1;
        }

        _request = "/gp/gpControl/setting/4/" + _parameter;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        switch (option)
        {
        case FR_240:
            _parameter = "0a";
            break;
        case FR_120:
            _parameter = "09";
            break;
        case FR_100:
            _parameter = "08";
            break;
        case FR_60:
            _parameter = "07";
            break;
        case FR_50:
            _parameter = "06";
            break;
        case FR_48:
            _parameter = "05";
            break;
        case FR_30:
            _parameter = "04";
            break;
        case FR_25:
            _parameter = "03";
            break;
        case FR_24:
            _parameter = "02";
            break;
        case FR_12p5:
            _parameter = "0b";
            break;
        case FR_15:
            _parameter = "01";
            break;
        case FR_12:
            _parameter = "00";
            break;
        default:
            _debug_port->println("Wrong parameter for setFrameRate");
            return -1;
        }

        _request = "camera/FS?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        switch (option)
        {
        case FR_240:
            _parameter = "0";
            break;
        case FR_120:
            _parameter = "1";
            break;
        case FR_100:
            _parameter = "2";
            break;
        case FR_90:
            _parameter = "3";
            break;
        case FR_80:
            _parameter = "4";
            break;
        case FR_60:
            _parameter = "5";
            break;
        case FR_50:
            _parameter = "6";
            break;
        case FR_48:
            _parameter = "7";
            break;
        case FR_30:
            _parameter = "8";
            break;
        case FR_25:
            _parameter = "9";
            break;
        default:
            _debug_port->println("Wrong parameter for setFrameRate");
            return -1;
        }

        _request = "/gp/gpControl/setting/3/" + _parameter;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        switch (option)
        {
        case NTSC:
            _parameter = "00";
            break;
        case PAL:
            _parameter = "01";
            break;
        default:
            _debug_port->println("Wrong parameter for setVideoEncoding");
            return -1;
        }

        _request = "camera/VM?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        switch (option)
        {
        case NTSC:
            _parameter = "0";
            break;
        case PAL:
            _parameter = "1";
            break;
        default:
            _debug_port->println("Wrong parameter for setVideoEncoding");
            return -1;
        }

        _request = "/gp/gpControl/setting/57/" + _parameter;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        switch (option)
        {
        case PR_11MP_WIDE:
            _parameter = "00";
            break;
        case PR_8MP_WIDE:
            _parameter = "01";
            break;
        case PR_5MP_WIDE:
            _parameter = "02";
            break;
        default:
            _debug_port->println("Wrong parameter for setPhotoResolution");
            return -1;
        }

        _request = "camera/PR?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        switch (option)
        {
        case PR_12MP_WIDE:
            _parameter = "0";
            break;
        case PR_12MP_LINEAR:
            _parameter = "10";
            break;
        case PR_12MP_MEDIUM:
            _parameter = "8";
            break;
        case PR_12MP_NARROW:
            _parameter = "9";
            break;
        case PR_7MP_WIDE:
            _parameter = "1";
            break;
        case PR_7MP_MEDIUM:
            _parameter = "2";
            break;
        case PR_5MP_WIDE:
            _parameter = "3";
            break;
        default:
            _debug_port->println("Wrong parameter for setPhotoResolution");
            return -1;
        }

        _request = "/gp/gpControl/setting/17/" + _parameter;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        switch (i_option)
        {
        case 60:
            _parameter = "3c";
            break;
        case 30:
            _parameter = "1e";
            break;
        case 10:
            _parameter = "0a";
            break;
        case 5:
            _parameter = "05";
            break;
        case 1:
            _parameter = "01";
            break;
        case 0: // 0.5
            _parameter = "00";
            break;
        default:
            _debug_port->println("Wrong parameter for setTimeLapseInterval");
            return -1;
        }

        _request = "camera/TI?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        switch (i_option)
        {
        case 60:
            _parameter = "6";
            break;
        case 30:
            _parameter = "5";
            break;
        case 10:
            _parameter = "4";
            break;
        case 5:
            _parameter = "3";
            break;
        case 1:
            _parameter = "1";
            break;
        case 0: // 0.5
            _parameter = "0";
            break;
        default:
            _debug_port->println("Wrong parameter for setTimeLapseInterval");
            return -1;
        }

        _request = "/gp/gpControl/setting/5/" + _parameter;
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        switch (option)
        {
        case 10:
            _parameter = "0a";
            break;
        case 5:
            _parameter = "05";
            break;
        case 3:
            _parameter = "03";
            break;
        case 0:
            _parameter = "00";
            break;
        default:
            _debug_port->println("Wrong parameter for setContinuousShot");
            return -1;
        }

        _request = "camera/CS?t=" + _pwd + "&p=%" + _parameter;
    }
    else if (_camera >= HERO4)
    {
        if (_debug)
        {
            _debug_port->println("Not supported by HERO4 and newer");
        }
        return false;
    }
    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        _request = "bacpac/LL?t=" + _pwd + "&p=%01";
    }
    else if (_camera >= HERO4)
    {
        _request = "/gp/gpControl/command/system/locate?p=1";
    }

    return sendHTTPRequest(_request);
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

    if (!isOn()) // camera is off
    {
        if (_debug)
        {
            _debug_port->println("turn on the camera first");
        }
        return false;
    }

    if (_camera == HERO3)
    {
        _request = "bacpac/LL?t=" + _pwd + "&p=%00";
    }
    else if (_camera >= HERO4)
    {
        _request = "/gp/gpControl/command/system/locate?p=0";
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        _request = "camera/DL?t=" + _pwd;
    }
    else if (_camera >= HERO4)
    {
        _request = "/gp/gpControl/command/storage/delete/last";
    }

    return sendHTTPRequest(_request);
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

    if (_camera == HERO3)
    {
        _request = "camera/DA?t=" + _pwd;
    }
    else if (_camera >= HERO4)
    {
        _request = "/gp/gpControl/command/command/storage/delete/all";
    }

    return sendHTTPRequest(_request);
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
        _debug_port->println(WiFi.SSID());
        _debug_port->print("Password:\t");
        _debug_port->println(_pwd);
        _debug_port->print("Camera:\t\tHERO");
        _debug_port->println(_camera);
        _debug_port->print("IP Address:\t");
        _debug_port->println(WiFi.localIP());
        _debug_port->print("RSSI:\t\t");
        _debug_port->print(WiFi.RSSI());
        _debug_port->println(" dBm");

        uint8_t my_mac[6];
        WiFi.macAddress(my_mac);
        _debug_port->print("My MAC:\t\t");
        _debug_port->print(my_mac[5], HEX);
        _debug_port->print(":");
        _debug_port->print(my_mac[4], HEX);
        _debug_port->print(":");
        _debug_port->print(my_mac[3], HEX);
        _debug_port->print(":");
        _debug_port->print(my_mac[2], HEX);
        _debug_port->print(":");
        _debug_port->print(my_mac[1], HEX);
        _debug_port->print(":");
        _debug_port->println(my_mac[0], HEX);

        /*
		uint8_t *gp_mac = WiFi.BSSID();
		_debug_port->print("GoPro MAC:\t");
		_debug_port->print(gp_mac[5], HEX);
		_debug_port->print(":");
		_debug_port->print(gp_mac[4], HEX);
		_debug_port->print(":");
		_debug_port->print(gp_mac[3], HEX);
		_debug_port->print(":");
		_debug_port->print(gp_mac[2], HEX);
		_debug_port->print(":");
		_debug_port->print(gp_mac[1], HEX);
		_debug_port->print(":");
		_debug_port->println(gp_mac[0], HEX);
		*/

        if (_camera >= HERO4)
        {
            _debug_port->print("Board Name:\t");
            _debug_port->println(_board_name);
        }

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

    if (_debug)
    {
        _debug_port->println("HTTP request: " + request);
    }

    _wifi_client.println("GET " + request + " HTTP/1.1");
    if (_camera == HERO3)
    {
        _wifi_client.println("Host: " + _host + ":" + _wifi_port);
    }
    else if (_camera >= HERO4)
    {
        _wifi_client.println("Host: " + _host);
    }
    _wifi_client.println("Connection: Keep-Alive");
    _wifi_client.println();

    uint16_t response = listenResponse();
    _wifi_client.stop();

    if (response == 200)
    {
        if (_debug)
        {
            _debug_port->println("Command: Accepted");
        }
        return true;
    }
    else if (response == 400)
    {
        if (_debug)
        {
            _debug_port->println("Command: Bad request");
        }
    }
    else if (response == 403)
    {
        if (_debug)
        {
            _debug_port->println("Command: Wrong password");
        }
    }
    else if (response == 410)
    {
        if (_debug)
        {
            _debug_port->println("Command: Failed");
        }
    }
    else
    {
        if (_debug)
        {
            _debug_port->println("Command: Other error");
        }
    }
    return -1;
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
        _request = "/gp/gpControl/command/wireless/pair/complete?success=1&deviceName=" + _board_name;
    }

    return sendHTTPRequest(_request);
}

uint16_t GoProControl::listenResponse()
{
    char incoming;
    char first_line[20];
    bool first_line_completed = false;
    uint8_t index = 0;

    if (_debug)
    {
        _debug_port->print("Waiting response");
    }

    uint32_t start_time = millis();
    while (_wifi_client.available() == 0 && start_time + MAX_WAIT_TIME > millis())
    {
        delay(5);
        if (_debug)
        {
            _debug_port->print(".");
        }
    }

    if (_debug)
    {
        _debug_port->println("\nStart response body");
    }

    while (_wifi_client.available() > 0)
    {
        incoming = _wifi_client.read();
        if (first_line_completed == false)
        {
            first_line[index++] = incoming;
            if (incoming == '\n')
            {
                first_line_completed = true;
            }
        }
        if (_debug)
        {
            _debug_port->print(incoming);
        }
    }

    if (_debug)
    {
        _debug_port->println("\nEnd response body");
    }

    if (first_line[0] == NULL) // empty response
    {
        if (_debug)
        {
            _debug_port->println("No response");
        }
        return false;
    }

    uint16_t response_code = atoi(splitString(first_line, 1));

    if (_debug)
    {
        _debug_port->print("Response code: ");
        _debug_port->println(response_code);
    }

    return response_code;
}

char *GoProControl::splitString(char str[], uint8_t index)
{
    uint8_t counter = 0;
    char *token = strtok(str, " ");
    while (token != NULL)
    {
        if (counter == index)
        {
            return token;
        }
        token = strtok(NULL, " ");
        counter++;
    }
    return NULL;
}
