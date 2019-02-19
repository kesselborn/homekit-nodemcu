PROGRAM = wifi_config

EXTRA_COMPONENTS = \
	extras/http-parser \
	extras/dhcpserver \
	${HOMEKIT_DEMOS_PATH}/components/wifi_config \
	${HOMEKIT_DEMOS_PATH}/components/wolfssl \
	${HOMEKIT_DEMOS_PATH}/components/cJSON \
	${HOMEKIT_DEMOS_PATH}/components/homekit

FLASH_SIZE ?= 32

EXTRA_CFLAGS += -I../.. -DHOMEKIT_SHORT_APPLE_UUIDS

include $(SDK_PATH)/common.mk

monitor:
	$(FILTEROUTPUT) --port $(ESPPORT) --baud 115200 --elf $(PROGRAM_OUT)
