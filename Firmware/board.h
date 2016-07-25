/*
 * board.h
 *
 *  Created on: 12 сент. 2015 г.
 *      Author: Kreyl
 */

#pragma once

#include <inttypes.h>

// ==== General ====
#define BOARD_NAME          "LedBandRxTx"
#define APP_NAME            "DalanRemoteCtrl"

// MCU type as defined in the ST header.
#define STM32L151xB

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ     12000000

#define SYS_TIM_CLK         (Clk.APB1FreqHz)
#define I2C1_ENABLED        FALSE
#define I2C_USE_SEMAPHORE   FALSE
#define ADC_REQUIRED        FALSE

#if 1 // ========================== GPIO =======================================
// PortMinTim_t: GPIO, Pin, Tim, TimChnl, invInverted, omPushPull, TopValue
// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     9
#define UART_RX_PIN     10
#define UART_AF         AF7 // for USART1 @ GPIOA

// Radio
#define CC_GPIO         GPIOA
#define CC_GDO2         2
#define CC_GDO0         3
#define CC_SCK          5
#define CC_MISO         6
#define CC_MOSI         7
#define CC_CS           4

// Buttons
#define BTN_GPIO        GPIOB
#define BTN_RED_PIN     9
#define BTN_BLUE_PIN    7
#define BTN_WHITE_PIN   6
#define BTN_OFF_PIN     3

// LED
#define LED_RED_CH      { GPIOB, 1, TIM3, 4, invNotInverted, omPushPull, 255 }
#define LED_GREEN_CH    { GPIOB, 0, TIM3, 3, invNotInverted, omPushPull, 255 }
#define LED_BLUE_CH     { GPIOB, 5, TIM3, 2, invNotInverted, omPushPull, 255 }

#endif // GPIO

#if 1 // =========================== SPI =======================================
#define CC_SPI          SPI1
#define CC_SPI_AF       AF5
#endif

#if 1 // ========================== USART ======================================
#define UART            USART1
#define UART_TX_REG     UART->DR
#define UART_RX_REG     UART->DR
#endif

#if 1 // =========================== DMA =======================================
#define STM32_DMA_REQUIRED  TRUE    // Always

// ==== Uart ====
#define UART_DMA_TX     STM32_DMA1_STREAM4
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   0   // Dummy

#endif // DMA
