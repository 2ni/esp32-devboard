# About
Compiling custom libesp32.a to be used with Platformio. [Based on this thread](https://github.com/espressif/arduino-esp32/issues/1142)

It's not yet fully working. There are still compiling issues when building the project in platformio with the adapted libesp32.a

# [Install esp-idf](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/) & python
```
cd /www
git clone git@github.com:espressif/esp-idf.git
cd esp-idf
pyenv virtualenv esp-idf
pyenv activate esp-idf
pip install -r requirements.txt
export IDF_PATH=/www/esp-idf
```

# [Install toolchain](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/macos-setup.html)
```
mkdir toolchain
cd toolchain
wget https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-80-g6c4433a-5.2.0.tar.gz
tar -xzf xtensa-esp32-elf-osx-1.22.0-80-g6c4433a-5.2.0.tar.gz
export PATH=/www/esp-idf/toolchain/xtensa-esp32-elf/bin:$PATH
```

# Use [example](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/#get-started-get-packages) to compile
```
cd /www/esp-idf
git submodule update --init
cp -r examples/get-started/hello_world .
cd hello_world
cp ~/.platformio/packages/framework-arduinoespressif32/tools/sdk/sdkconfig .
<change settings, eg CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL>

make
```

optionally you can use make menuconfig "config serial port in Serial flasher config/Default serial port"

if needed:

```
make flash
make monitor
```

# Use libesp32.a in platformio
```
cd ~/.platformio/packages/framework-arduinoespressif32/tools/sdk/lib/
cp libesp32.a libesp32.a.orig
cp www/esp-idf/hello_world/build/esp32/libesp32.a .
```

Run `pio run -t clean` before compiling your project in platformio
