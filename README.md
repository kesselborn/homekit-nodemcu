# Building esp homekit demos on mac os x

This is a repo with a docker build container for building the [nodemcu homekit demo apps](https://github.com/maximkulkin/esp-homekit-demo). Installing the cross compiling tool chain on macos is a pain, therefore this work around.

## Preparations



- create `wifi.h` and adjust it: `cp wifi.h.sample wifi.h` 

- download the driver for your nodemcu from https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers

- install the flash tool with `pip install esptool` 

- build the build container with `docker-compose build`

  This will take ages at the first run ... it'll compile all examples

- copy the prebuilt firmwares to the host system

      cp /home/nodemcu/esp-open-rtos/bootloader/firmware_prebuilt/*.bin /host
  

  

## Use build container

Within the build container, you can (re-)compile all examples as explained at
https://github.com/maximkulkin/esp-homekit-demo#usage (Point 7)

Within the `/host` directory, build the led sample  but changing into the docker container and building the binary:

    docker-compose run sdk
    make -C firmware/led all

## Flashing binary to nodemcu

On the host system (i.e. not in the docker container) flash the binary like this:

    BIN=firmware/led/firmwareled.bin make flash
