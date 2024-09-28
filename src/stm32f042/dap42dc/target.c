/*
 * Copyright (c) 2016, Devan Lai
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/crs.h>
#include <libopencm3/stm32/gpio.h>

#include "target.h"
#include "config.h"

/* Reconfigure processor settings */
void cpu_setup(void) {

}

/* Set STM32 to 48 MHz. */
void clock_setup(void) {
    rcc_clock_setup_in_hsi48_out_48mhz();

    // Trim from USB sync frame
    crs_autotrim_usb_enable();
    rcc_set_usbclk_source(RCC_HSI48);
}

static void button_setup(void) {
    /* Enable GPIOB clock. */
    rcc_periph_clock_enable(RCC_GPIOB);

    /* Set PB8 to an input */
    gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO8);
}

void gpio_setup(void) {
    /*
      LED0 (R), 1 (G), 2 (B) on PA5, PA6, PA7, open drain
      TX, RX (MCU-side) on PA2, PA3
      TGT_RST on PA4
      TGT_SWDIO, TGT_SWCLK on PA0, PA1
    */

    /* Enable GPIOA and GPIOB clocks. */
    rcc_periph_clock_enable(RCC_GPIOA);

    /* Setup LEDs as open-drain outputs */
    gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_LOW,
                            GPIO5 | GPIO6 | GPIO7);

    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                    GPIO5 | GPIO6 | GPIO7);
    button_setup();
}

void target_console_init(void) {
    /* Enable UART clock */
    rcc_periph_clock_enable(CONSOLE_USART_CLOCK);

    /* Setup GPIO pins for UART2 */
    gpio_mode_setup(CONSOLE_USART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, CONSOLE_USART_GPIO_PINS);
    gpio_set_af(CONSOLE_USART_GPIO_PORT, CONSOLE_USART_GPIO_AF, CONSOLE_USART_GPIO_PINS);
}

void led_num(uint8_t value) {
    if (value & 0x4) {
        gpio_clear(GPIOA, GPIO5);
    } else {
        gpio_set(GPIOA, GPIO5);
    }
    if (value & 0x2) {
        gpio_clear(GPIOA, GPIO6);
    } else {
        gpio_set(GPIOA, GPIO6);
    }
    if (value & 0x1) {
        gpio_clear(GPIOA, GPIO7);
    } else {
        gpio_set(GPIOA, GPIO7);
    }
}
