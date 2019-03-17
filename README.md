# Summary
This is a custom dev board based on the ESP32 and platformio. It can be connected to the computer via usb for programming and includes base periphery.

The goal is to have one single board which includes some base functionality.
There is no need to add any wires or add-ons for prototyping; depending on your needs of course.

Main features:
- Gyro / Accelerometer (MPU-6050)
- 1 Main Output Control to steer eg a small motor
- 1 push button (debounced by hardware)
- 1 rotary/push button (debounced by hardware)
- Micro SD Slot
- IR-Sender / Receiver
- Audio out (via PCM5102 / I2S)
- RFM69/RFM95 slot
- Micro SD card slot
- SPI connector
- I2C connector
- USB connector (based on CH340)
- 64Mb Flash

# Hardware
see [hardware/](hardware) or https://easyeda.com/vkbs/esp32-developer

# Wifi
add your custom file creds.h in [src/common/](src/common) similar to the one below:

```
static const char* WIFI_CREDS[] = {"ssid1", "pw1",\
                                   "ssid2", "pw2",\
                                   NULL};
```

# Base functions and definitions
There are a couple of base functions for debugging or connecting to wifi. See [src/common/base_functions.h](src/common/base_functions.h)

| Function          | Description                                   | Example                  |
|-------------------|-----------------------------------------------|--------------------------|
| DL()              | Output text to terminal with linefeed         | DL("Hello World")        |
| DF()              | Output text with variables to terminal        | DF("argument a:%u\n", a) |
| connect_to_wifi() | Connects the ESP to wifi according to creds.h |                          |

All the board specific GPIO's are also defined in that file.

# Examples
There are a couple of [examples](examples) which can be "activated" and run as follows:
```
./activate.py modules/examples/blink.cpp
make st
```

make st means "serial/terminal". It will detect on which usb your board is connected, compile the activated code and run a terminal.

Dependencies can be searched and installed with

```
pio lib search <sth>
pio lib install <id>
```

or cloned into the lib directory.

| Example    | Description                                                                  | Dependencies        |
|------------|------------------------------------------------------------------------------|---------------------|
| [blink.cpp](modules/examples/blink.cpp)  | Blinking the onboard LED at startup and when using the push button or rotary | [Rotary by buxtronix](https://github.com/buxtronix/arduino/blob/master/libraries/Rotary) |
| [buzzer.cpp](modules/examples/buzzer.cpp) | Plays RTTTL strings on a buzzer connected to CN1                             | [ESP32-RTTTL](https://github.com/2ni/ESP32-RTTTL)       |
| [spiffs.cpp](modules/examples/spiffs.cpp) | Save data persistently to SPIFFS (in this example ntp timestamp)             | [TIMY](https://github.com/2ni/timy)            |
