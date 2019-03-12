FROM ubuntu
# https://github.com/pfalcon/esp-open-sdk
RUN apt-get update
RUN echo "Europe/Berlin" > /etc/timezone
RUN DEBIAN_FRONTEND=noninteractive apt-get install -qy make unrar-free autoconf automake libtool gcc g++ gperf \
    flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial \
    sed git unzip bash help2man wget bzip2 libtool-bin python3-pip vim && pip3 install esptool
RUN dpkg-reconfigure -f noninteractive tzdata

RUN useradd -ms /bin/bash nodemcu
USER nodemcu

ENV PATH="/home/nodemcu/esp-open-sdk/xtensa-lx106-elf/bin:$PATH" \
    SDK_PATH=/home/nodemcu/esp-open-rtos \
    SPI_SIZE=1M \
    SPI_MODE=dout \
    SPI_SPEED=40 \
    HOMEKIT_DEMOS_PATH="/home/nodemcu/esp-homekit-demo" \
    C_INCLUDE_PATH=/home/nodemcu/esp-open-rtos/include:/home/nodemcu/rboot

WORKDIR /home/nodemcu
RUN git clone --recurse-submodules https://github.com/maximkulkin/esp-homekit-demo.git
RUN git clone --recurse-submodules https://github.com/pfalcon/esp-open-sdk.git && make -C esp-open-sdk -j6 toolchain esptool libhal STANDALONE=n || true
RUN git clone --recurse-submodules https://github.com/Superhouse/esp-open-rtos.git

# https://github.com/maximkulkin/esp-homekit-demo#usage
RUN git clone --recurse-submodules https://github.com/raburton/esptool2.git && make -C esptool2 -j6
RUN git clone --recurse-submodules https://github.com/raburton/rboot.git && C_INCLUDE_PATH=/home/nodemcu/esp-open-rtos/include:/home/nodemcu/rboot:/home/nodemcu/esp-open-rtos/bootloader make -C rboot -j6 all
RUN git clone --recurse-submodules https://github.com/raburton/rboot-sample.git
RUN git clone --recurse-submodules https://github.com/HomeACcessoryKid/life-cycle-manager.git

COPY wifi.h /home/nodemcu/esp-homekit-demo
# COPY program1.ld /home/nodemcu/esp-open-rtos/ld/program1.ld
RUN cat /home/nodemcu/esp-open-rtos/ld/program.ld|sed 's/irom0_0_seg :.*org = 0x40202010, len = (1M - 0x2010)/irom0_0_seg :                               org = 0x4028D010, len = (0xf5000 - 0x2010)/g' > /home/nodemcu/esp-open-rtos/ld/program1.ld

ENV C_INCLUDE_PATH=/home/nodemcu/esp-open-rtos:/home/nodemcu/esp-open-rtos/bootloader/:/home/nodemcu/esp-open-rtos/bootloader/rboot:/home/nodemcu/esp-open-rtos/bootloader/rboot/appcode

CMD ["sh", "-c", "while true; do sleep 3600; done;"]
