/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

// Define the pinout for our ROM signal connections

// The address lines are sequential, starting at GP0 through GP14, total of 15 address bits (max ROM 23256: 32Kx8)
#define ROM_ADDR_COUNT    15
#define ROM_A0      0

// The data lines are sequential, starting at GP15 for 8 bits
#define ROM_D0      15

//unused            23          Not Available on Pico
//unused            24          Not Available on Pico
//unused            25          Not Available on Pico

//unused            26

#define ROM_CE      27          // wired to ROM pin 18 (CE): normally active LOW, but on this ROM it is active HIGH!
#define ROM_OE      28          // wired to ROM pin 20 (OE): active LOW


// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#define LED_ON_DELAY_MS 50


// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

// Init the GPIOs to talk to an ROM
// For the moment, we are hardcoded talking to a 2732 (4K x 8)
void ROM_initGpio()
{
    // Init the !CS first and disable it
    gpio_init(ROM_CE);
    gpio_set_dir(ROM_CE, GPIO_OUT);
    gpio_put(ROM_CE, 1);

    // Init OE to the disabled state
    gpio_init(ROM_OE);
    gpio_set_dir(ROM_OE, GPIO_OUT);
    gpio_put(ROM_OE, 1);

    // Init the data bus to be all inputs, with pullups enabled
    for (uint32_t i=0; i<8; i++) {
        uint32_t io = ROM_D0+i;
        gpio_init(io);
        gpio_set_dir(io, GPIO_IN);
        if (i&1) {
            gpio_pull_up(io);
        }
        else {
            gpio_pull_down(io);
        }
    }

    // Init the address bus to be all outputs
    for (uint32_t i=0; i<ROM_ADDR_COUNT; i++) {
        uint32_t io = ROM_A0+i;
        gpio_init(io);
        gpio_set_dir(io, GPIO_OUT);
        gpio_put(io, 1);
    }
}

uint8_t contents[32768] __attribute__ ((aligned (4096)));

int main()
{
    pico_led_init();

    ROM_initGpio();

    uint flashcount = 4;
    while (--flashcount) {
        pico_set_led(true);
        sleep_ms(10);
        pico_set_led(false);
        sleep_ms(50);
    }

    // Select the ROM by driving its CE to '1' (active high for this ROM)
    gpio_put(ROM_CE, 1);

    // Let it drive the data bus
    gpio_put(ROM_OE, 0);

    bool firstTime = true;
    while (1) {
        pico_set_led(1);
        for (uint32_t addr = 0; addr<0x1000; addr++) {
            gpio_put_masked(0x7FFF, addr);
            sleep_us(1);
            uint32_t raw = gpio_get_all();
            uint8_t data = (raw >> ROM_D0) & 0xff;
            if (!firstTime) {
                if (data != contents[addr]) {
                    __breakpoint();
                }
            }
            contents[addr] = data;
        }

        for (uint32_t addr = 0; addr<0x800; addr++) {
            if (contents[addr] != contents[addr+0x800]) {
                __breakpoint();
            }
        }
        firstTime = false;
        sleep_ms(50);
        pico_set_led(0);
        sleep_ms(100);
    }
}
