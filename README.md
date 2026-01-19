# ESP32 Smart Light

A system that is powered by ESP32 and controls 2 LED strips and a neopixel ring using clapping and the web!

![image](https://blueprint.hackclub.com/user-attachments/representations/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODU2MDgsInB1ciI6ImJsb2JfaWQifX0=--528c51d16b7ff8ef595a77a7c20971b1ceecad92/eyJfcmFpbHMiOnsiZGF0YSI6eyJmb3JtYXQiOiJqcGciLCJyZXNpemVfdG9fbGltaXQiOlsyMDAwLDIwMDBdLCJjb252ZXJ0Ijoid2VicCIsInNhdmVyIjp7InF1YWxpdHkiOjgwLCJzdHJpcCI6dHJ1ZX19LCJwdXIiOiJ2YXJpYXRpb24ifX0=--15ca3815f01a5683e19ea0585d2eef9af9e441d7/20260119_123537.jpg)

## Why did I do this?

I wanted to create a system that could be controlled using my new Big sound module, so I started thinking and came up with this interesting idea!

## Features

- Web interface
- It has a buzzer and an RGB LED for notification
- You can clap to turn the lights on/off
- You have a neopixel ring that you can set up to 10+ mods on

## Scripts

In total, the script has about 650 lines, but the website, functions,... I'm only in this script. So just upload it via Arduino IDE to ESP32 and everything should work!!

### Home page:
![image](https://blueprint.hackclub.com/user-attachments/representations/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODU3NzEsInB1ciI6ImJsb2JfaWQifX0=--f3e7d57e276f627ac5d69fc8cdc1b8edb24b478e/eyJfcmFpbHMiOnsiZGF0YSI6eyJmb3JtYXQiOiJwbmciLCJyZXNpemVfdG9fbGltaXQiOlsyMDAwLDIwMDBdLCJjb252ZXJ0Ijoid2VicCIsInNhdmVyIjp7InF1YWxpdHkiOjgwLCJzdHJpcCI6dHJ1ZX19LCJwdXIiOiJ2YXJpYXRpb24ifX0=--0f85faa91c373105a0f317054e965c1f47e93a37/Sn%C3%ADmek%20obrazovky%202026-01-19%20163954.png)

## How it works?

- **Clap control:** 1x clap toggles the LED strip, 2x claps toggle the ring, 3x claps change the ring mode.
- **Physical button:** Click toggles the strip, Double click changes brightness, Triple click swaps ring modes. Long press turns the whole system On/Off.
- **Web UI:** Change 12 ring modes (Matrix, Fire, Rainbow, etc.), 3 strip effects, and brightness.
- **Status LED:** Shows WiFi status and gives color feedback (Green/Red/Blue) when you change settings.
- **Auto Demo:** Randomly cycles through all ring effects every 10 seconds....

### Libraries:

```
- Adafruit_NeoPixel.h
- OneButton.h
- WiFi.h
- WebServer.h
- Preferences.h
```


## 3D models

Here is a view of the top and bottom of the case:

![image](https://github.com/mavory/ESP32-Smart-light/blob/main/Photos/Sn%C3%ADmek%20obrazovky%202026-01-18%20170256.png?raw=true)

![image](https://github.com/mavory/ESP32-Smart-light/blob/main/Photos/Sn%C3%ADmek%20obrazovky%202026-01-18%20170338.png?raw=true)

## Wiring Diagram

![image](https://blueprint.hackclub.com/user-attachments/representations/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODU4MzQsInB1ciI6ImJsb2JfaWQifX0=--cf0010f35ee36b1f048a7a5dac775518477c9f7d/eyJfcmFpbHMiOnsiZGF0YSI6eyJmb3JtYXQiOiJwbmciLCJyZXNpemVfdG9fbGltaXQiOlsyMDAwLDIwMDBdLCJjb252ZXJ0Ijoid2VicCIsInNhdmVyIjp7InF1YWxpdHkiOjgwLCJzdHJpcCI6dHJ1ZX19LCJwdXIiOiJ2YXJpYXRpb24ifX0=--0f85faa91c373105a0f317054e965c1f47e93a37/image.png)

## There is the final video of how it works!

_Here is a youtube video where you can see how it works:_ [Link](https://youtu.be/)
