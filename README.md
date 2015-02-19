# esp8266_tpm2net_ws2812
This is a custom firmware for the esp8266 wifi module that will drive a strand of ws2812 LEDs using the TPM2NET protocol. I was successful at physically driving 180 LEDs (12x15 matrix) using this firmware and the pixelcontroller (v2.1.0-RC1) & glediator (v2.0.3) software. I have also successfully tested it by sending the data for 512 LEDs (32x16). This is based on work by Charles (cnlohr) and Frans (Frans-Willem) ...

# How to connect hardware

Connect GPIO_0 of ESP8266 to DATA. You should have a 100p cap across GPIO0 to GND.

#UPDATE:2/17/2015
(multiblock) split frame support has been added to the code and test have confirmed a 16x32 (512 RGB LEDS) matrix using the jinx led host software. http://www.esp8266.com/viewtopic.php?f=6&t=1441

#How to build a build environment.

see this site from cnlohr: https://github.com/cnlohr/ws2812esp8266
