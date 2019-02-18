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

WORKDIR /home/nodemcu
RUN git clone https://github.com/maximkulkin/esp-homekit-demo.git
WORKDIR /home/nodemcu/esp-homekit-demo
RUN git submodule update --init --recursive

WORKDIR /home/nodemcu
RUN git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
WORKDIR /home/nodemcu/esp-open-sdk
# https://github.com/maximkulkin/esp-homekit-demo#usage
RUN make toolchain esptool libhal STANDALONE=n
ENV PATH="/home/nodemcu/esp-open-sdk/xtensa-lx106-elf/bin:$PATH"

WORKDIR /home/nodemcu
RUN git clone --recursive https://github.com/Superhouse/esp-open-rtos.git
ENV SDK_PATH=/home/nodemcu/esp-open-rtos

WORKDIR /home/nodemcu/esp-homekit-demo
COPY wifi.h .
RUN for d in examples/*; do make -C $d all; done

WORKDIR /host
