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
#define APP_NAME            "RedTransmitter"

// MCU type as defined in the ST header.
#define STM32L151xB

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ     12000000

#define SYS_TIM_CLK         (Clk.APB1FreqHz)
#define I2C1_ENABLED        FALSE
#define I2C_USE_SEMAPHORE   FALSE
#define ADC_REQUIRED        TRUE

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

// "Color indication" switch
#define SWITCH_PIN      GPIOB, 5

// LED
//#define LED_RED_CH      { GPIOB, 1, TIM3, 4, invNotInverted, omPushPull, 255 }
//#define LED_GREEN_CH    { GPIOB, 0, TIM3, 3, invNotInverted, omPushPull, 255 }
//#define LED_BLUE_CH     { GPIOB, 5, TIM3, 2, invNotInverted, omPushPull, 255 }

#define LED0_PIN        GPIOA, 8,  omPushPull
#define LED1_PIN        GPIOA, 11, omPushPull
#define LED2_PIN        GPIOA, 12, omPushPull
#define LED3_PIN        GPIOA, 15, omPushPull

// ADC inputs
#define R0_PIN          GPIOB, 12
#define R1_PIN          GPIOB, 13
#define R2_PIN          GPIOB, 14
#define R3_PIN          GPIOB, 15

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
