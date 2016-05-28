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

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#define PRODUCT_NAME "DAP42C"
#define REMAP_USB 0
#define DEFAULT_BAUDRATE 115200

#define CONSOLE_USART USART2
#define CONSOLE_TX_BUFFER_SIZE 128
#define CONSOLE_RX_BUFFER_SIZE 128

#define CONSOLE_USART_GPIO_PORT GPIOA
#define CONSOLE_USART_GPIO_PINS (GPIO2|GPIO3)
#define CONSOLE_USART_GPIO_AF   GPIO_AF1
#define CONSOLE_USART_MODE USART_MODE_TX_RX
#define CONSOLE_USART_CLOCK RCC_USART2

#define CONSOLE_USART_IRQ_NAME  usart2_isr
#define CONSOLE_USART_NVIC_LINE NVIC_USART2_IRQ

#define nBOOT0_GPIO_CLOCK RCC_GPIOF
#define nBOOT0_GPIO_PORT GPIOF
#define nBOOT0_GPIO_PIN  GPIO11

#include <libopencm3/stm32/usart.h>
/* Workaround for non-commonalized STM32F0 USART code */
#ifndef USART_STOPBITS_1
#define USART_STOPBITS_1 USART_CR2_STOP_1_0BIT
#endif

#ifndef USART_STOPBITS_2
#define USART_STOPBITS_2 USART_CR2_STOP_2_0BIT
#endif

#ifndef USART_SR_RXNE
#define USART_SR_RXNE USART_ISR_RXNE
#endif

#ifndef USART_SR_TXE
#define USART_SR_TXE USART_ISR_TXE
#endif

/* Word size for usart_recv and usart_send */
typedef uint8_t usart_word_t;

#define LED_OPEN_DRAIN         0

#endif