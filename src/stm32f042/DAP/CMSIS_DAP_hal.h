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

/*
    Portions of this file are derived from:

    Dapper Mime - an open-source CMSIS-DAP implementation
    HAL for STM32F0xx2
    this file is used by the mbed CMSIS-DAP routines

    Copyright (C) 2014 Peter Lawrence

    Permission is hereby granted, free of charge, to any person obtaining a 
    copy of this software and associated documentation files (the "Software"), 
    to deal in the Software without restriction, including without limitation 
    the rights to use, copy, modify, merge, publish, distribute, sublicense, 
    and/or sell copies of the Software, and to permit persons to whom the 
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in 
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/

#ifndef __DAP_HAL_H__
#define __DAP_HAL_H__

#include <libopencm3/stm32/gpio.h>
#include "DAP/CMSIS_DAP_config.h"

//#define SBDAP 1

/*
SWD functionality
see: https://arm-software.github.io/CMSIS_5/latest/DAP/html/group__DAP__Config__PortIO__gr.html
*/

/*
Setup SWD I/O pins: SWCLK, SWDIO, and nRESET.
Configures the DAP Hardware I/O pins for Serial Wire Debug (SWD) mode:

    SWCLK, SWDIO, nRESET to output mode and set to default high level.
*/
static __inline void PORT_SWD_SETUP (void)
{
    GPIO_BSRR(SWCLK_GPIO_PORT) = SWCLK_GPIO_PIN;
    GPIO_BSRR(SWDIO_GPIO_PORT) = SWDIO_GPIO_PIN;
    GPIO_BSRR(nRESET_GPIO_PORT) = nRESET_GPIO_PIN;

#if defined(SBDAP)
    // set SWDIO buffer to output mode
    GPIO_BSRR(SWDIR_GPIO_PORT) = SWDIR_GPIO_PIN;
    // do not invoke NXP LPC boot loader
    GPIO_BSRR(CTL_GPIO_PORT) = CTL_GPIO_PIN;
#endif
  
    gpio_set_output_options(SWCLK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, SWCLK_GPIO_PIN);
    gpio_set_output_options(SWDIO_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, SWDIO_GPIO_PIN);
    // gpio_set_output_options(nRESET_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, nRESET_GPIO_PIN);
    // gpio_set_output_options(nRESET_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_LOW, nRESET_GPIO_PIN);

#if defined(SBDAP)
    gpio_set_output_options(SWDIR_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, SWDIR_GPIO_PIN);
    //gpio_set_output_options(CTL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, CTL_GPIO_PIN);
#endif
    

    gpio_mode_setup(SWCLK_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SWCLK_GPIO_PIN);
    gpio_mode_setup(SWDIO_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SWDIO_GPIO_PIN);
    // gpio_mode_setup(nRESET_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, nRESET_GPIO_PIN);
    // gpio_mode_setup(nRESET_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, nRESET_GPIO_PIN);

#if defined(SBDAP)
    gpio_mode_setup(SWDIR_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SWDIR_GPIO_PIN);
    //gpio_mode_setup(CTL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CTL_GPIO_PIN);
#endif
}

/*
Disable SWD I/O Pins.
Disables the DAP Hardware I/O pins which configures:

    SWCLK, SWDIO, (SWO), nRESET to High-Z mode.
*/
static __inline void PORT_OFF (void)
{
#if defined(SBDAP)
    GPIO_BRR(SWDIO_GPIO_PORT) = SWDIO_GPIO_PIN;
    GPIO_BRR(SWCLK_GPIO_PORT) = SWCLK_GPIO_PIN;
    GPIO_BSRR(nRESET_GPIO_PORT) = nRESET_GPIO_PIN;
    gpio_mode_setup(SWDIO_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, SWDIO_GPIO_PIN);
    
    // set SWDIO buffer to input mode
    GPIO_BRR(SWDIR_GPIO_PORT) = SWDIR_GPIO_PIN;

#else
    GPIO_BRR(SWDIO_GPIO_PORT) = SWDIO_GPIO_PIN;
    GPIO_BRR(SWCLK_GPIO_PORT) = SWCLK_GPIO_PIN;
    gpio_mode_setup(SWDIO_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, SWDIO_GPIO_PIN);
    gpio_mode_setup(SWCLK_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, SWCLK_GPIO_PIN);
#endif
}

/*
SWCLK/TCK I/O pin: Set Output to High.
Set the SWCLK/TCK DAP hardware I/O pin to high level.
*/
static __inline void PIN_SWCLK_TCK_SET (void)
{
    GPIO_BSRR(SWCLK_GPIO_PORT) = SWCLK_GPIO_PIN;
}

/*
SWCLK/TCK I/O pin: Set Output to Low.
Set the SWCLK/TCK DAP hardware I/O pin to low level.
*/
static __inline void PIN_SWCLK_TCK_CLR (void)
{
    GPIO_BRR(SWCLK_GPIO_PORT) = SWCLK_GPIO_PIN;
}

/*
SWDIO/TMS I/O pin: Get Input.
Returns
    Current status of the SWDIO/TMS DAP hardware I/O pin. 
*/
static __inline uint32_t PIN_SWDIO_TMS_IN  (void)
{
    return (GPIO_IDR(SWDIO_GPIO_PORT) & SWDIO_GPIO_PIN) ? 0x1 : 0x0;
}

/*
SWDIO/TMS I/O pin: Set Output to High.
Set the SWDIO/TMS DAP hardware I/O pin to high level.
*/
static __inline void PIN_SWDIO_TMS_SET (void)
{
    GPIO_BSRR(SWDIO_GPIO_PORT) = SWDIO_GPIO_PIN;
}

/*
SWDIO/TMS I/O pin: Set Output to Low.
Set the SWDIO/TMS DAP hardware I/O pin to low level.
*/
static __inline void PIN_SWDIO_TMS_CLR (void)
{
    GPIO_BRR(SWDIO_GPIO_PORT) = SWDIO_GPIO_PIN;
}

/*
SWDIO I/O pin: Get Input (used in SWD mode only).
Returns
    Current status of the SWDIO DAP hardware I/O pin. 
*/
static __inline uint32_t PIN_SWDIO_IN (void)
{
    return (GPIO_IDR(SWDIO_GPIO_PORT) & SWDIO_GPIO_PIN) ? 0x1 : 0x0;
}

/*
SWDIO I/O pin: Set Output (used in SWD mode only).
Parameters
    bit	Output value for the SWDIO DAP hardware I/O pin. 
*/
static __inline void PIN_SWDIO_OUT (uint32_t bit)
{
    if (bit & 1) {
        GPIO_BSRR(SWDIO_GPIO_PORT) = SWDIO_GPIO_PIN;
    } else {
        GPIO_BRR(SWDIO_GPIO_PORT) = SWDIO_GPIO_PIN;
    }
}

/*
SWDIO I/O pin: Switch to Output mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to output mode. This function is called prior PIN_SWDIO_OUT function calls.
*/
static __inline void PIN_SWDIO_OUT_ENABLE  (void)
{ 
#if defined(SBDAP)
    // set SWDIO buffer to output mode
    GPIO_BSRR(SWDIR_GPIO_PORT) = SWDIR_GPIO_PIN;
#endif

    GPIO_MODER(SWDIO_GPIO_PORT) &= ~( (0x3 << (SWDIO_GPIO_PIN_NUM * 2)) );
    GPIO_MODER(SWDIO_GPIO_PORT) |=  ( (0x1 << (SWDIO_GPIO_PIN_NUM * 2)) );
}

/*
SWDIO I/O pin: Switch to Input mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to input mode. This function is called prior PIN_SWDIO_IN function calls.
*/
static __inline void PIN_SWDIO_OUT_DISABLE (void)
{
    GPIO_MODER(SWDIO_GPIO_PORT) &= ~( (0x3 << (SWDIO_GPIO_PIN_NUM * 2)) );

#if defined(SBDAP)
    // set SWDIO buffer to input mode
    GPIO_BRR(SWDIR_GPIO_PORT) = SWDIR_GPIO_PIN;
#endif
}

/*
SWCLK/TCK I/O pin: Get Input.
Returns
    Current status of the SWCLK/TCK DAP hardware I/O pin. 
*/
static __inline uint32_t PIN_SWCLK_TCK_IN  (void) {
	return (GPIO_IDR(SWCLK_GPIO_PORT) & SWCLK_GPIO_PIN) ? 0x1 : 0x0;
}

/*
nRESET I/O pin: Get Input.
Returns
    Current status of the nRESET DAP hardware I/O pin. 
*/
static __inline uint32_t PIN_nRESET_IN  (void) {
#if defined(nRESET_GPIO_PORT) && defined(nRESET_GPIO_PIN)
	return (GPIO_IDR(nRESET_GPIO_PORT) & nRESET_GPIO_PIN) ? 0x1 : 0x0;
#else
    return 1;
#endif
}

/*
nRESET I/O pin: Set Output.
Parameters
    bit	target device hardware reset pin status:
        0: issue a device hardware reset.
        1: release device hardware reset.
*/
static __inline void PIN_nRESET_OUT (uint32_t bit) {
#if defined(nRESET_GPIO_PORT) && defined(nRESET_GPIO_PIN)
    if (bit & 0x1) {
        GPIO_BSRR(nRESET_GPIO_PORT) = nRESET_GPIO_PIN;
    } else {
        GPIO_BRR(nRESET_GPIO_PORT) = nRESET_GPIO_PIN;
    }
#endif
}

/*
Debug Unit: Set status of Connected LED.
Parameters
    bit	status of the Connect LED.
        1: Connect LED ON: debugger is connected to CMSIS-DAP Debug Unit.
        0: Connect LED OFF: debugger is not connected to CMSIS-DAP Debug Unit.
*/
static __inline void LED_CONNECTED_OUT (uint32_t bit) {
#if defined(LED_CON_GPIO_PORT) && defined(LED_CON_GPIO_PIN)
    if ((bit & 0x1) ^ LED_OPEN_DRAIN) {
        gpio_set(LED_CON_GPIO_PORT, LED_CON_GPIO_PIN);
    } else {
        gpio_clear(LED_CON_GPIO_PORT, LED_CON_GPIO_PIN);
    }
#endif
}

/*
Debug Unit: Set status Target Running LED.
Parameters
    bit	status of the Target Running LED.
        1: Target Running LED ON: program execution in target started.
        0: Target Running LED OFF: program execution in target stopped.
*/
static __inline void LED_RUNNING_OUT (uint32_t bit) {
#if defined(LED_RUN_GPIO_PORT) && defined(LED_RUN_GPIO_PIN)
    if ((bit & 0x1) ^ LED_OPEN_DRAIN) {
        gpio_set(LED_RUN_GPIO_PORT, LED_RUN_GPIO_PIN);
    } else {
        gpio_clear(LED_RUN_GPIO_PORT, LED_RUN_GPIO_PIN);
    }
#endif
}

// show USB activity
static __inline void LED_ACTIVITY_OUT (uint32_t bit) {
#if defined(LED_ACT_GPIO_PORT) && defined(LED_ACT_GPIO_PIN)
    if ((bit & 0x1) ^ LED_OPEN_DRAIN) {
        gpio_set(LED_ACT_GPIO_PORT, LED_ACT_GPIO_PIN);
    } else {
        gpio_clear(LED_ACT_GPIO_PORT, LED_ACT_GPIO_PIN);
    }
#endif
}

/*
Setup of the Debug Unit I/O pins and LEDs (called when Debug Unit is initialized).
This function performs the initialization of the CMSIS-DAP Hardware I/O Pins and the Status LEDs.
In detail the operation of Hardware I/O and LED pins are enabled and set:
    I/O clock system enabled.
    all I/O pins: input buffer enabled, output pins are set to HighZ mode.
    for nTRST, nRESET a weak pull-up (if available) is enabled.
    LED output pins are enabled and LEDs are turned off.
*/
static __inline void DAP_SETUP (void) {
    LED_ACTIVITY_OUT(0);
    LED_RUNNING_OUT(0);
    LED_CONNECTED_OUT(0);

#if defined(nRESET_GPIO_PORT) && defined(nRESET_GPIO_PIN)
    // Configure nRESET as an open-drain output with pull-up resistor 
    GPIO_BSRR(nRESET_GPIO_PORT) = nRESET_GPIO_PIN;
    gpio_set_output_options(nRESET_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_LOW, nRESET_GPIO_PIN);
    gpio_mode_setup(nRESET_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, nRESET_GPIO_PIN);
#endif

#if defined(SBDAP)
    // do not invoke NXP LPC boot loader
    GPIO_BSRR(CTL_GPIO_PORT) = CTL_GPIO_PIN;
    gpio_set_output_options(CTL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, CTL_GPIO_PIN);
    gpio_mode_setup(CTL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CTL_GPIO_PIN);
#endif
}

/*
  JTAG-only functionality (not used in this application)
*/

static __inline void PORT_JTAG_SETUP (void) {}
static __inline uint32_t PIN_TDI_IN  (void) {  return 0; }
static __inline void     PIN_TDI_OUT (uint32_t bit) { (void)bit; }
static __inline uint32_t PIN_TDO_IN (void) {  return 0; }
static __inline uint32_t PIN_nTRST_IN (void) {  return 0; }
static __inline void     PIN_nTRST_OUT  (uint32_t bit) { (void)bit; }

/*
  other functionality (not used in this application)
*/

static __inline uint32_t RESET_TARGET (void) { return 0; }

#endif
