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

enum mode
{
    mode_first = 0,
    VIDEO_MODE,
    PHOTO_MODE,
    BURST_MODE,
    TIMELAPSE_MODE,
    TIMER_MODE,
    PLAY_HDMI,
    MULTISHOT_MODE, //Hero4/5/6 modes:
    mode_last
};

enum orientation
{
    orientation_first = mode_last + 1,
    ORIENTATION_UP,
    ORIENTATION_DOWN,
    ORIENTATION_AUTO, // Hero4/5/6
    orientation_last
};

enum video_resolution
{
    video_resolution_first = orientation_last + 1,
    VR_WVGA60,
    VR_WVGA120,
    VR_720_30,
    VR_720_60,
    VR_960_30,
    VR_960_48,
    VR_1080_30,
    //Resolutions Hero4/5/6
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
    video_resolution_last
};

enum photo_resolution
{
    photo_resolution_first = video_resolution_last + 1,
    PR_12mpW,
    PR_11mpW,
    PR_8mpW,
    PR_7mpW,
    PR_5mpW,
    //Photo Resolution Hero 4/5/6
    PR_12MP_Wide,
    PR_12MP_Linear,
    PR_12MP_Medium,
    PR_12MP_Narrow,
    PR_7MP_Wide,
    PR_7MP_Medium,
    PR_5MP_Wide,
    photo_resolution_last
};

enum frame_rate
{
    frame_rate_first = photo_resolution_last + 1,
    FPS12,
    FPS15,
    FPS12p5,
    FPS24,
    FPS25,
    FPS30,
    FPS48,
    FPS50,
    FPS60,
    FPS100,
    FPS120,
    FPS240,
    // Frame rates Hero4/5/6
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
    frame_rate_last
};

enum fov
{
    fov_first = frame_rate_last + 1,
    WIDE_FOV,
    MEDIUM_FOV,
    NARROW_FOV,
    LINEAR_FOV, // Hero4/5/6
    fov_last
};

enum video_encoding
{
    video_encoding_first = fov_last + 1,
    NTSC,
    PAL,
    video_encoding_last
};
