/*
 * SnsPins.h
 *
 *  Created on: 17 ���. 2015 �.
 *      Author: Kreyl
 */

/* ================ Documentation =================
 * There are several (may be 1) groups of sensors (say, buttons and USB connection).
 * There is GPIO and Pin data for every sensor.
 *
 */


#ifndef PINSNSSETTINGS_H_
#define PINSNSSETTINGS_H_

#include "ch.h"
#include "hal.h"
#include "kl_lib.h"
#include "main.h" // App.thd here
#include "evt_mask.h"

#define SNS_POLL_PERIOD_MS  27

enum PinSnsState_t {pssLo, pssHi, pssRising, pssFalling};
typedef void (*ftVoidPSnsStLen)(PinSnsState_t *PState, uint32_t Len);

// Single pin setup data
struct PinSns_t {
    GPIO_TypeDef *PGpio;
    uint16_t Pin;
    PinPullUpDown_t Pud;
    ftVoidPSnsStLen Postprocessor;
    uint8_t Name;
    void Init() const { PinSetupIn(PGpio, Pin, Pud); }
    void Off()  const { PinSetupAnalog(PGpio, Pin);  }
    bool IsHi() const { return PinIsSet(PGpio, Pin); }
};

// ================================= Settings ==================================
// Buttons handler
extern void ProcessButtons(PinSnsState_t *PState, uint32_t Len);

#define BUTTONS_CNT     3   // Setup appropriately. Required for buttons handler
// Select convenient names
enum BtnName_t {btnOff=0, btnMid=1, btnFull=2};

const PinSns_t PinSns[] = {
        // Buttons
        {GPIOB, 7, pudPullUp, ProcessButtons, btnFull},
        {GPIOB, 6, pudPullUp, ProcessButtons, btnMid},
        {GPIOB, 3, pudPullUp, ProcessButtons, btnOff},
};
#define PIN_SNS_CNT     countof(PinSns)

#endif /* PINSNSSETTINGS_H_ */
