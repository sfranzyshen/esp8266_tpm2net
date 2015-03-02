#Notice: Currently, this is a work in progress ...
it will not compile ... to find working examples of code (and firmware) ... please refer to this thread on the esp8266 fourm ... http://www.esp8266.com/viewtopic.php?f=6&t=1441 Thank You!

# esp8266_tpm2net
esp8266_tpm2net is a custom firmware for the esp8266 wifi module that will drive a strand of addressable RGB LED pixels using the TPM2NET protocol.  We are planning support for the following chipsets ... as well as a tpm2net to tpm2ser gateway for use with other MCUs (arduino, stm32f1, ect.) connected via the UART. The goal for this project is to create a solid rgb led pixel driver based on the esp8266 wifi module for the control of 512-1024 rgb pixels.

#Planned Supported Chipsets
WS2801, WS2811, WS2812, LPD6803, LPD8803, LPD8806, LPD8809, APA102, APA104, PCA9685, TLC5940, ...
as well as a tpm2net to tpm2ser gateway with firmware(s) for the Arduino & Stm32f1 platforms using the FastLED library ...

#How to build a build environment.

see this site from cnlohr: https://github.com/cnlohr/ws2812esp8266

#Thank You!
The work here has been possible because of the help from various projects ...

https://github.com/cnlohr/ws2812esp8266

https://github.com/Frans-Willem/EspLightNode

https://github.com/popsodav/esp8266-lpd6803

https://github.com/3s1d/WiFiYaala

https://github.com/markusgritsch/nodemcu-firmware


