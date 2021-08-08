/**
 * \file            lwgsm_ll_stm32f429zi_nucleo.c
 * \brief           Low-level communication with GSM device for STM32F429ZI-Nucleo using DMA
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwGSM - Lightweight GSM-AT library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v0.1.0
 */

/*
 * Default UART configuration is:
 *
 * UART:                USART6
 * STM32 TX (GSM RX):   GPIOC, GPIO_PIN_6
 * STM32 RX (GSM TX):   GPIOC, GPIO_PIN_7
 * RESET:               GPIOC, GPIO_PIN_5
 *
 * USART_DMA:           DMA2
 * USART_DMA_STREAM:    DMA_STREAM_1
 * USART_DMA_CHANNEL:   DMA_CHANNEL_5
 */
#include "lwgsm/lwgsm.h"
#include "lwgsm/lwgsm_mem.h"
#include "lwgsm/lwgsm_input.h"
#include "system/lwgsm_ll.h"

#if !__DOXYGEN__

#include "stm32l4xx_ll_lpuart.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_dma.h"


/* USART */
#define LWGSM_USART                           LPUART1
#define LWGSM_USART_CLK                       LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1)
#define LWGSM_USART_IRQ                       LPUART1_IRQn
#define LWGSM_USART_IRQHANDLER                LPUART1_IRQHandler
// #define LWGSM_USART_RDR_NAME                  DR

/* DMA settings */
#define LWGSM_USART_DMA                       DMA2
#define LWGSM_USART_DMA_CLK                   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2)
// #define LWGSM_USART_DMA_RX_STREAM             LL_DMA_STREAM_1
#define LWGSM_USART_DMA_RX_CH                 LL_DMA_CHANNEL_7
#define LWGSM_USART_DMA_RX_IRQ                DMA2_Channel7_IRQn
#define LWGSM_USART_DMA_RX_IRQHANDLER         DMA2_Channel7_IRQHandler

/* DMA flags management */
#define LWGSM_USART_DMA_RX_IS_TC              LL_DMA_IsActiveFlag_TC7(LWGSM_USART_DMA)
#define LWGSM_USART_DMA_RX_IS_HT              LL_DMA_IsActiveFlag_HT7(LWGSM_USART_DMA)
#define LWGSM_USART_DMA_RX_CLEAR_TC           LL_DMA_ClearFlag_TC7(LWGSM_USART_DMA)
#define LWGSM_USART_DMA_RX_CLEAR_HT           LL_DMA_ClearFlag_HT7(LWGSM_USART_DMA)

/* USART TX PIN */
#define LWGSM_USART_TX_PORT_CLK               LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define LWGSM_USART_TX_PORT                   GPIOA
#define LWGSM_USART_TX_PIN                    LL_GPIO_PIN_2
#define LWGSM_USART_TX_PIN_AF                 LL_GPIO_AF_8

/* USART RX PIN */
#define LWGSM_USART_RX_PORT_CLK               LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define LWGSM_USART_RX_PORT                   GPIOA
#define LWGSM_USART_RX_PIN                    LL_GPIO_PIN_3
#define LWGSM_USART_RX_PIN_AF                 LL_GPIO_AF_8

/* RESET PIN */
#define LWGSM_RESET_PORT_CLK                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define LWGSM_RESET_PORT                      GPIOA
#define LWGSM_RESET_PIN                       LL_GPIO_PIN_11


// LP UART defines
#define DATAWIDTH                             LL_LPUART_DATAWIDTH_8B
#define DIRECTION_TX_RX                       LL_LPUART_DIRECTION_TX_RX
#define HWCONTROL                             LL_LPUART_HWCONTROL_NONE
// #define OVERSAMPLING                          LL_LPUART_OVERSAMPLING_16
#define PARITY                                LL_LPUART_PARITY_NONE
#define STOPBITS                              LL_LPUART_STOPBITS_1
#define DMA_RX_REQ_NUM                        LL_DMA_REQUEST_4 



/* Include STM32 generic driver */
#include "../system/lwgsm_ll_stm32_L4.c"

#endif /* !__DOXYGEN__ */
