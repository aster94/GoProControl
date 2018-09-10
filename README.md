# GoPro Control Arduino
This is a library to interface with GoPro cameras, just press a button and turn on your GoPro action camera using an Arduino!

![gopro and arduino](https://image.ibb.co/cGRb4p/1.jpg)

## Supported boards:
- ESP8266
- ESP32
- any arduino boards (UNO, nano, 101, etc.) attached to an ESP8266 (ESP01) using [this library](https://github.com/bportaluri/WiFiEsp)

## Supported cameras:
- HERO3+

I made the library with a style which would be quite easy to add other cameras (not only gopro). I will be very happy to accept pull requests and have as collaborator other people

## Installation

Use the arduino library manager or download directly from github

# Story
Have you ever thought about the possibility to control your action camera with your Arduino? Well if you are here because you googled that here you may be interested in watching [this video](https://youtu.be/PuM-ZQ2tMW0)

[![Alt text](https://img.youtube.com/vi/PuM-ZQ2tMW0/0.jpg)](https://www.youtube.com/watch?v=PuM-ZQ2tMW0 "click to watch it on youtube")

In the video I take a picture, change the mode from photo to video, start and stop a video, I delete a file and finally I turn off the GoPro. But these are only a few of the functions that are possible to do with this library. For example you could change the FOV (field of view), the frame rate, the photo and video resolution, turn the localization on or off, rotate the orientation of the camera and more

## Reference

all the commands came from: https://github.com/KonradIT/goprowifihack

The idea cames from another gopro library: https://github.com/agdl/GoPRO which works only on arduino WiFi boards and only with gopro HERO3.
