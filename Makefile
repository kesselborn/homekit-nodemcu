PORT ?= /dev/cu.SLAB_USBtoUART
flash:
	esptool.py -p ${PORT} --baud 115200 write_flash -fs 32m -fm qio -ff 40m 0x0 rboot.bin 0x1000 blank_config.bin 0x2000 $${BIN:?set \$$BIN variable to the binary you want to flash}
