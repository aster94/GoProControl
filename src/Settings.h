/*
Settings.h

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

#define TIME_OUT_CONNECTION 5000

enum camera
{
    HERO = 1,
    HERO2,
    HERO3,
    HERO4,
    HERO5,
    HERO6,
    HERO7
};

//The above settings must be between a *_first and *_last member
enum mode
{
    mode_first = 0,
    VIDEO_MODE,
    PHOTO_MODE,
    BURST_MODE,
    TIMELAPSE_MODE,
    TIMER_MODE,
    PLAY_HDMI_MODE,
    MULTISHOT_MODE,
    mode_last
};

enum orientation
{
    orientation_first = mode_last + 1,
    ORIENTATION_UP,
    ORIENTATION_DOWN,
    ORIENTATION_AUTO,
    orientation_last
};

enum video_resolution
{
    video_resolution_first = orientation_last + 1,
    // resolutions for HERO 4/5/6/7
    VR_4K,
    VR_2K,
    VR_2K_SuperView,
    VR_1440p,
    VR_1080p_SuperView,
    VR_1080p,
    VR_960p,
    VR_720p_SuperView,
    VR_720p,
    VR_WVGA,
    // in the HERO3 and earlier the video resolution and frame rate are set together
    VR_1080p_30,
    VR_960p_48,
    VR_960p_30,
    VR_720p_60,
    VR_720p_30,
    VR_WVGA_120,
    VR_WVGA_60,
    video_resolution_last
};

enum video_fov
{
    video_fov_first = video_resolution_last + 1,
    WIDE_FOV,
    MEDIUM_FOV,
    NARROW_FOV,
    LINEAR_FOV,
    video_fov_last
};

enum frame_rate
{
    frame_rate_first = video_fov_last + 1,
    FR_240,
    FR_120,
    FR_100,
    FR_90,
    FR_80,
    FR_60,
    FR_50,
    FR_48,
    FR_30,
    FR_25,
    FR_24,
    FR_15,
    FR_12p5,
    FR_12,
    frame_rate_last
};

enum video_encoding
{
    video_encoding_first = frame_rate_last + 1,
    NTSC,
    PAL,
    video_encoding_last
};

enum photo_resolution
{
    photo_resolution_first = video_encoding_last + 1,
    PR_12MP_WIDE,
    PR_12MP_MEDIUM,
    PR_12MP_NARROW,
    PR_12MP_LINEAR,
    PR_11MP_WIDE,
    PR_8MP_WIDE,
    PR_8MP_MEDIUM,
    PR_7MP_WIDE,
    PR_7MP_MEDIUM,
    PR_5MP_WIDE,
    PR_5MP_MEDIUM,
    photo_resolution_last
};
