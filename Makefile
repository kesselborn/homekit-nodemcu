PORT ?= /dev/cu.usbserial-A5XK3RJT

# for rsp8266 with 1mb flash only ... sometimes this needs a few tries
flash-plug:
	esptool.py --before no_reset_no_sync --chip esp8266 --port ${PORT} --baud 115000 erase_flash
	FLASH_SIZE=8 HOMEKIT_SPI_FLASH_BASE_ADDR=0x7a000 FLASH_MODE=dout esptool.py --after hard_reset --before no_reset_no_sync --chip esp8266 --port ${PORT} --baud 115000 write_flash -fs 1MB -fm dout -ff 40m 0x0 rboot.bin 0x1000 blank_config.bin 0x2000 $${BIN:?set \$$BIN variable to the binary you want to flash}

flash-nodemcu-as-plug:
	while ! esptool.py -p ${PORT} --baud 115200 erase_flash; do echo "trying erasing again ..."; (set +x; sleep 2); done
	while ! esptool.py -p ${PORT} --baud 115200 write_flash -fs 1MB -fm dout -ff 40m 0x0 rboot.bin 0x1000 blank_config.bin 0x2000 $${BIN:?set \$$BIN variable to the binary you want to flash};  do echo "trying flashing again ..."; (set +x; sleep 2); done

test:
	esptool.py --before no_reset_no_sync --chip esp8266 --port ${PORT} --baud 115000 erase_flash
	FLASH_SIZE=8 HOMEKIT_SPI_FLASH_BASE_ADDR=0x7a000 FLASH_MODE=dout esptool.py --after hard_reset --before no_reset_no_sync --chip esp8266 --port ${PORT} --baud 115000 write_flash -fs 1MB -fm dout -ff 40m 0x0 http_ota.bin

flash-nodemcu::
	while ! esptool.py -p ${PORT} --baud 115200 erase_flash; do echo "trying erasing again ..."; (set +x; sleep 2); done
	while ! esptool.py -p ${PORT} --baud 115200 write_flash -fs 32m -fm qio -ff 40m 0x0 rboot.bin 0x1000 blank_config.bin 0x2000 $${BIN:?set \$$BIN variable to the binary you want to flash};  do echo "trying flashing again ..."; (set +x; sleep 2); done

build-fake-plug:
	EXTRA_CFLAGS="-DFAKE_PLUG -DVERSION=$${VERSION:?set VERSION}" make -j6 -C firmware/kesselPLUG
	cp firmware/kesselPLUG/firmware/kesselPLUG.bin kesselplug-${VERSION}.bin
