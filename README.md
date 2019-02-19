# Building esp homekit demos on mac os x

This is a repo with a docker build container for building the [nodemcu homekit demo apps](https://github.com/maximkulkin/esp-homekit-demo). Installing the cross compiling tool chain on macos is a pain, therefore this work around.



## Preparations

Instructions are for nodemcu esp12* ... esp32 boards need different handling: https://github.com/maximkulkin/esp-homekit-demo#esp32 

- create `wifi.h` and adjust it: `cp wifi.h.sample wifi.h` 

- download the driver for your nodemcu from https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers

- install the flash tool with `pip install esptool` 

- build the build container with `docker-compose build`

  This will take ages at the first run ... it'll compile all examples

- start and jump into the docker container and copy the prebuilt firmwares to the host system

      docker-compose run sdk
      cp /home/nodemcu/esp-open-rtos/bootloader/firmware_prebuilt/*.bin /host

- copy all examples you would like to try out to your host directory

      cp -a /home/nodemcu/esp-homekit-demo/examples /host/contrib-firmwares

## Use build container

Within the build container, you can (re-)compile all examples as explained at
https://github.com/maximkulkin/esp-homekit-demo#usage (Point 7)

Within the `/host` directory, build the led sample by changing into the docker container and building the binary:

    docker-compose run sdk
    make -C firmware/led all

## Build other examples in `/host` directory

If you want to copy examples from `/home/nodemcu/esp-homekit-demo/examples` you must adjust the `Makefile` of the respective example ... see

    vimdiff /host/firmware/led/Makefile /home/nodemcu/esp-homekit-demo/examples/led/Makefile

for necessary changes.

## Flashing binary to nodemcu

On the host system (i.e. not in the docker container) flash the binary like this:

    BIN=firmware/led/firmwareled.bin make flash

if you copied over the examples and want to flash one of those do:

    BIN=contrib-firmwares/<name>/firmware/<name>.bin