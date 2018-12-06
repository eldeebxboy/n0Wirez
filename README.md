# Wireless Thermostat
**Authors** 
*Saif Abid, Jiri Kuba*

## HW setup ESP-01s
ESP-01s is using ESP8266 microcontroller.
![ESP-01s](./docs/esp-01s-esp8266-wifi-module.jpg) ![ESP-01s pinout](./docs/ESP8266-PINOUT.png)

To flash ESP-01s - pull GPIO0 to GND, pull RST to GND and release RST.

## Build setup
Install [ArduinoIDE](https://www.arduino.cc/en/Main/Software) and add ESP8266 support as described (here)[https://github.com/esp8266/Arduino].

### Faster builds without ArduinoIDE
Clone [makeEspArduino](https://github.com/plerup/makeEspArduino) to $HOME directory.
 
Command for build and flash:

``` make -f ~/makeEspArduino/makeEspArduino.mk UPLOAD_SPEED=921600 flash ```
