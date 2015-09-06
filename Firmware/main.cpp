/*
 * File:   main.cpp
 * Author: Kreyl
 * Project: Armlet2South
 *
 * Created on Feb 05, 2013, 20:27
 */

#include "kl_lib.h"
#include "clocking.h"
#include "ch.h"
#include "hal.h"
#include "main.h"
#include "buttons.h"
#include "led.h"

App_t App;
LedRGB_t Led { {GPIOB, 1, TIM3, 4}, {GPIOB, 0, TIM3, 3}, {GPIOB, 5, TIM3, 2} };

int main(void) {
    // ==== Init clock system ====
//    Clk.SetupBusDividers(ahbDiv2, apbDiv1, apbDiv1);
    Clk.UpdateFreqValues();

    // ==== Init OS ====
    halInit();
    chSysInit();

    // ==== Init Hard & Soft ====
    JtagDisable();
    Uart.Init(115200);
    Uart.Printf("\r%S %S", APP_NAME, APP_VERSION);
    Clk.PrintFreqs();

    App.InitThread();

    // Rx or Tx?
    PinSetupIn(GPIOA, 15, pudPullUp);
    chThdSleepMilliseconds(1);
    App.IsTransmitter = !PinIsSet(GPIOA, 15);
    PinSetupAnalog(GPIOA, 15);

    if(App.IsTransmitter) {
        Led.Init();
        Led.SetColor(clDarkGreen);
        PinSensors.Init();
    }

    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    // ==== Main cycle ====
    while(true) {
        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);
        if(IsTransmitter) {
            if(EvtMsk & EVTMSK_BUTTONS) {
                BtnEvtInfo_t EInfo;
                while(ButtonEvtBuf.Get(&EInfo) == OK) {
    //                Uart.Printf("\rEinfo: %u, %u", EInfo.Type, EInfo.BtnID[0]);
                    switch((BtnName_t)EInfo.Name[0]) {
                        case btnOff:
                            Brightness = 0;
                            Led.SetColor(clDarkGreen);
                            break;

                        case btnMid:
                            Brightness = 72;
                            Led.SetColor(clDarkYellow);
                            break;

                        case btnFull:
                            Brightness = 255;
                            Led.SetColor(clDarkRed);
                            break;
                    } // switch btn
                } // while get info
            } // if buttons
        } // if transmitter
    } // while true
} // Main thread
