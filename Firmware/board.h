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
#define APP_NAME            "Buttons1234"

// MCU type as defined in the ST header.
#define STM32L151xB

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ     12000000

#define SYS_TIM_CLK         (Clk.APB1FreqHz)

#define SIMPLESENSORS_ENABLED   TRUE
#define BUTTONS_ENABLED         TRUE
#define I2C1_ENABLED            FALSE
#define I2C_USE_SEMAPHORE       FALSE
#define ADC_REQUIRED            FALSE

#if 1 // ========================== GPIO =======================================
// PortMinTim_t: GPIO, Pin, Tim, TimChnl, invInverted, omPushPull, TopValue
// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     9
#define UART_RX_PIN     10

// Radio: SPI, PGpio, Sck, Miso, Mosi, Cs, Gdo0
#define CC_Setup0       SPI1, GPIOA, 5,6,7, 4, 3

// Buttons
#define BTN_1_PIN       GPIOB, 9, pudPullUp
#define BTN_2_PIN       GPIOB, 7, pudPullUp
#define BTN_3_PIN       GPIOB, 6, pudPullUp
#define BTN_4_PIN       GPIOB, 3, pudPullUp

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
#define CMD_UART        USART1
#define UART_TXBUF_SZ   1024
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Clock divider: clock is generated from the APB2
#define ADC_CLK_DIVIDER     adcDiv2

// ADC channels
#define ADC_R0_CHNL         18
#define ADC_R1_CHNL         19
#define ADC_R2_CHNL         20
#define ADC_R3_CHNL         21

#define ADC_CHANNELS        { ADC_R0_CHNL, ADC_R1_CHNL, ADC_R2_CHNL, ADC_R3_CHNL }
#define ADC_CHANNEL_CNT     4   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast48Cycles // see kl_adc.h
#define ADC_OVERSAMPLING_RATIO  4   // 1 (no oversampling), 2, 4, 8, 16, 32, 64, 128, 256
#endif

#if 1 // =========================== DMA =======================================
#define STM32_DMA_REQUIRED  TRUE    // Always

// ==== Uart ====
#define UART_DMA_TX     STM32_DMA1_STREAM4
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   0   // Dummy

#if ADC_REQUIRED
#define ADC_DMA         STM32_DMA1_STREAM1
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* DMA1 Stream1 Channel0 */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA
