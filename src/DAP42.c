/*
 * Copyright (c) 2015, Devan Lai
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

#include <stdio.h>
#include <string.h>

#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/iwdg.h>

#include "config.h"
#include "target.h"

#include "USB/composite_usb_conf.h"
#include "USB/cdc.h"
#include "USB/vcdc.h"
#include "USB/dfu.h"

#include "DAP/app.h"
#include "DAP/CMSIS_DAP_hal.h"
#include "DFU/DFU.h"

#include "CAN/slcan.h"

#include "tick.h"
#include "retarget.h"
#include "console.h"

extern void initialise_monitor_handles(void);

static inline uint32_t millis(void) {
    return get_ticks();
}

static inline void wait_ms(uint32_t duration_ms) {
    uint32_t now = millis();
    uint32_t end = now + duration_ms;
    if (end < now) {
        end = 0xFFFFFFFFU - end;
        while (millis() >= now) {
            __asm__("NOP");
        }
    }

    while (millis() < end) {
        __asm__("NOP");
    }
}

static uint32_t usb_timer = 0;
static void on_usb_activity(void) {
    usb_timer = 1000;
}

static bool do_reset_to_dfu = false;
static void on_dfu_request(void) {
    do_reset_to_dfu = true;
}

static inline void set_target_state(bool reset, bool enter_bootloader) {
    /*
     * For transitioning from true/true to false/false, clear CTL first to ensure
     * the MCU does not enter the bootloader.
     */
    PIN_CTL_OUT(enter_bootloader);
    PIN_nRESET_OUT(!reset);
}

static bool do_deferred_set_target_state = false;
static uint32_t set_target_state_timer_start;
static bool set_target_state_reset;
static bool set_target_state_enter_bootloader;
static void on_cdc_set_control_line_state(bool dtr, bool rts) {
    /*
     * At least Linux and Windows set true/true by default when an app opens
     * the channel, so forwarding these values directly to set_target_state
     * would cause the target MCU to reset right away, even when one simply wants
     * to capture UART output without resetting the target. Therefore, this
     * combination must be ignored here.
     *
     * When NXP Flash Magic wants to reset the target, it first sets true/true,
     * followed by either true/false or false/true, followed by false/false.
     * Thus, the condition dtr != rts indicates it's time to reset the target.
     */
    if (dtr != rts) {
        /* Reset now */
        set_target_state(true, true);

        /* Defer setting the requested state */
        do_deferred_set_target_state = true;
        set_target_state_timer_start = get_ticks();
        set_target_state_reset = dtr;
        set_target_state_enter_bootloader = rts;
    }
    else if (!dtr) {
        /* Parameters are false/false */
        do_deferred_set_target_state = false;
        set_target_state(false, false);
    }
}

int main(void) {
    if (DFU_AVAILABLE) {
        DFU_maybe_jump_to_bootloader();
    }

    cpu_setup();
    clock_setup();
    tick_setup(1000);
    gpio_setup();
    led_num(0);

    if (CDC_AVAILABLE) {
        console_setup(DEFAULT_BAUDRATE);
    }

    if (SEMIHOSTING) {
        initialise_monitor_handles();
    }
    else if (VCDC_AVAILABLE) {
        retarget(STDOUT_FILENO, VIRTUAL_USART);
        retarget(STDERR_FILENO, VIRTUAL_USART);
    } else if (CDC_AVAILABLE) {
        retarget(STDOUT_FILENO, CONSOLE_USART);
        retarget(STDERR_FILENO, CONSOLE_USART);
    }

    led_num(1);

    {
        char serial[USB_SERIAL_NUM_LENGTH+1];
        desig_get_unique_id_as_string(serial, USB_SERIAL_NUM_LENGTH+1);
        cmp_set_usb_serial_number(serial);
    }

    usbd_device* usbd_dev = cmp_usb_setup();
    DAP_app_setup(usbd_dev, &on_dfu_request);

    if (CDC_AVAILABLE) {
        cdc_uart_app_setup(usbd_dev,
                           &on_cdc_set_control_line_state,
                           &on_usb_activity,
                           &on_usb_activity);
        cdc_uart_app_set_timeout(1);
    }

    if (VCDC_AVAILABLE) {
        vcdc_app_setup(usbd_dev, &on_usb_activity, &on_usb_activity);
    }

    if (DFU_AVAILABLE) {
        dfu_setup(usbd_dev, &on_dfu_request);
    }

    if (CAN_RX_AVAILABLE && VCDC_AVAILABLE) {
        slcan_app_setup(500000, MODE_RESET);
    }

    tick_start();

    LED_SELFTEST();

    /* Enable the watchdog to enable DFU recovery from bad firmware images */
    iwdg_set_period_ms(1000);
    iwdg_start();

    while (1) {
        iwdg_reset();
        usbd_poll(usbd_dev);

        if (CDC_AVAILABLE) {
            cdc_uart_app_update();
        }

        if (CAN_RX_AVAILABLE && VCDC_AVAILABLE) {
            slcan_app_update();
        }

        if (VCDC_AVAILABLE) {
            vcdc_app_update();
        }


        // Handle DAP
        bool dap_active = DAP_app_update();
        if (dap_active) {
            usb_timer = 1000;
        } else if (do_reset_to_dfu && DFU_AVAILABLE) {
            /* Blink 3 times to indicate reset */
            int x;
            for (x=0; x < 3; x++) {
                iwdg_reset();
                led_num(7);
                wait_ms(150);
                led_num(0);
                wait_ms(150);
                iwdg_reset();
            }

            DFU_reset_and_jump_to_bootloader();
        }

        if (usb_timer > 0) {
            usb_timer--;
            LED_ACTIVITY_OUT(1);
        } else {
            LED_ACTIVITY_OUT(0);
        }

        bool timer_elapsed = (get_ticks() - set_target_state_timer_start) >= 25;
        if (do_deferred_set_target_state && timer_elapsed) {
            do_deferred_set_target_state = false;
            set_target_state(set_target_state_reset, set_target_state_enter_bootloader);
        }
    }

    return 0;
}
