/*
 * Example of using esp-homekit library to control
 * a simple $5 Sonoff Basic using HomeKit.
 * The esp-wifi-config library is also used in this
 * example. This means you don't have to specify
 * your network's SSID and password before building.
 *
 * In order to flash the sonoff basic you will have to
 * have a 3,3v (logic level) FTDI adapter.
 *
 * To flash this example connect 3,3v, TX, RX, GND
 * in this order, beginning in the (square) pin header
 * next to the button.
 * Next hold down the button and connect the FTDI adapter
 * to your computer. The sonoff is now in flash mode and
 * you can flash the custom firmware.
 *
 * WARNING: Do not connect the sonoff to AC while it's
 * connected to the FTDI adapter! This may fry your
 * computer and sonoff.
 *
 */

#include <sysparam.h>
#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <wifi_config.h>
#include <rboot-api.h>

#include "button.h"

// The GPIO pin that is connected to the relay on the Sonoff Basic.
const int relay_gpio = 15;    // blitzwolf
// The GPIO pin that is connected to the LED on the Sonoff Basic.
const int led_gpio = 14; // blitzwolf
const int internal_led_gpio = 2;
// The GPIO pin that is oconnected to the button on the Sonoff Basic.
#ifdef FAKE_PLUG
const int button_gpio = 0; // flash button
#else
const int button_gpio = 13; // flash button
#endif

#define STR_VALUE(arg) #arg
#ifndef VERSION
#pragma GCC error "please set VERSION with EXTRA_FLAGS=-DVERSION=x.y.z"
#endif

#define display_name "Steckdose"

void switch_on_callback(homekit_characteristic_t *_ch, homekit_value_t on, void *context);
void button_callback(uint8_t gpio, button_event_t event);

void relay_write(bool on) {
    gpio_write(relay_gpio, on ? 1 : 0);
}

void led_write(bool on) {
    gpio_write(led_gpio, on ? 0 : 1);
}

void internal_led_write(bool on) {
    gpio_write(internal_led_gpio, !on ? 0 : 1);
}


void reset_configuration_task() {
    //Flash the LED first before we start the reset
    for (int i=0; i<3; i++) {
        led_write(true);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        led_write(false);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    
    printf("Resetting Wifi Config\n");
    wifi_config_reset();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Resetting Repo Config\n");
    sysparam_set_string("ota_repo", "");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Resetting HomeKit Config\n");
    homekit_server_reset();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Restarting\n");
    sdk_system_restart();
    vTaskDelete(NULL);
}

void reset_configuration() {
    printf("Resetting configuration\n");
    xTaskCreate(reset_configuration_task, "Reset configuration", 256, NULL, 2, NULL);
}

homekit_characteristic_t switch_on = HOMEKIT_CHARACTERISTIC_(
    ON, false, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback)
);

void gpio_init() {
    gpio_enable(led_gpio, GPIO_OUTPUT);
    led_write(switch_on.value.bool_value);
    gpio_enable(internal_led_gpio, GPIO_OUTPUT);
    internal_led_write(!switch_on.value.bool_value);
    gpio_enable(relay_gpio, GPIO_OUTPUT);
    relay_write(switch_on.value.bool_value);
}

void switch_on_callback(homekit_characteristic_t *_ch, homekit_value_t on, void *context) {
    led_write(switch_on.value.bool_value);
    internal_led_write(!switch_on.value.bool_value);
    relay_write(switch_on.value.bool_value);
}

void button_callback(uint8_t gpio, button_event_t event) {
    switch (event) {
        case button_event_single_press:
            printf("Toggling relay\n");
            switch_on.value.bool_value = !switch_on.value.bool_value;

            relay_write(switch_on.value.bool_value);
            led_write(switch_on.value.bool_value);
            internal_led_write(!switch_on.value.bool_value);

            homekit_characteristic_notify(&switch_on, switch_on.value);
            break;
        case button_event_long_press:
            printf("Triggering OTA\n");
	    rboot_set_temp_rom(1);
	    sdk_system_restart();
            break;
        case button_event_super_long_press:
            printf("Resetting configuration\n");
	    reset_configuration_task();
	    rboot_set_temp_rom(1);
	    sdk_system_restart();
            break;
        default:
            printf("Unknown button event: %d\n", event);
    }
}

void switch_identify_task(void *_args) {
    // We identify the Sonoff by Flashing it's LED.
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            led_write(true);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            led_write(false);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    led_write(false);

    vTaskDelete(NULL);
}

void switch_identify(homekit_value_t _value) {
    printf("Switch identify\n");
    xTaskCreate(switch_identify_task, "Switch identify", 128, NULL, 2, NULL);
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, display_name);

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            &name,
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "tuya"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456789AZ"),
            HOMEKIT_CHARACTERISTIC(MODEL, "plug"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, STR_VALUE(VERSION)),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, switch_identify),
            NULL
        }),
        HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, display_name),
            &switch_on,
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "111-11-111"
};

void on_wifi_ready() {
    homekit_server_init(&config);
}

void create_accessory_name() {
    uint8_t macaddr[6];
    sdk_wifi_get_macaddr(STATION_IF, macaddr);
    
    int name_len = snprintf(NULL, 0, "%s-%02X%02X%02X", display_name,
                            macaddr[3], macaddr[4], macaddr[5]);
    char *name_value = malloc(name_len+1);
    snprintf(name_value, name_len+1, "%s-%02X%02X%02X", display_name,
             macaddr[3], macaddr[4], macaddr[5]);
    
    name.value = HOMEKIT_STRING(name_value);
}

void user_init(void) {
    uart_set_baud(0, 115200);

    create_accessory_name();
    
    wifi_config_init("sonoff-switch", NULL, on_wifi_ready);
    gpio_init();

    if (button_create(button_gpio, 0, 3000, 6000, button_callback)) {
        printf("Failed to initialize button\n");
    }
    homekit_server_init(&config);
}
