# Building esp homekit demos on mac os x

This is a repo with a docker build container for building nodemcu homekit apps. Installing the cross compiling tool chain on macos is a pain, therefore this work around.

## Preparations



- create `wifi.h` and adjust it: `cp wifi.h.sample wifi.h` 

- download the driver for your nodemcu from https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers

- install the flash tool with `pip install esptool` 

- build the build container with `docker-compose build`

  This will take ages at the first run ... it'll compile all examples

  

## Use build container

Within the build container, you can (re-)compile all examples as explained at
https://github.com/maximkulkin/esp-homekit-demo#usage (Point 7)



After building, call `sync` in the container which will copy all binaries to the current directory on the host.



## Flashing binary to nodemcu

Flash the nodemcu with your bin file (here: led.bin) by calling

    BIN=led.bin make flash