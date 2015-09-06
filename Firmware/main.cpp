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
#include "Sequences.h"
#include "radio_lvl1.h"

App_t App;
LedRGB_t Led { {GPIOB, 1, TIM3, 4}, {GPIOB, 0, TIM3, 3}, {GPIOB, 5, TIM3, 2} };
PinOutputPWM_t<(BRT_FULL-1), invNotInverted, omPushPull> Output {GPIOB, 8, TIM16, 1};

int main(void) {
    // ==== Init clock system ====
    Clk.SetupBusDividers(ahbDiv2, apbDiv1, apbDiv1);
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
        Led.SetColor(clDarkYellow);
        PinSensors.Init();
    }
    else {
        Output.Init();
        Output.SetFrequencyHz(2520);
        App.SignalEvt(EVTMSK_NEW_BRT);
    }

    if(Radio.Init() != OK) {
        if(App.IsTransmitter) Led.StartSequence(lsqFailure);
        chThdSleepMilliseconds(2700);
    }

    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    // ==== Main cycle ====
    while(true) {
        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);
        if(EvtMsk & EVTMSK_BUTTONS) {
            BtnEvtInfo_t EInfo;
            while(ButtonEvtBuf.Get(&EInfo) == OK) {
//                Uart.Printf("\rEinfo: %u, %u", EInfo.Type, EInfo.BtnID[0]);
                switch((BtnName_t)EInfo.Name[0]) {
                    case btnOff:
                        Brightness = BRT_LOW;
                        Led.SetColor(clDarkGreen);
                        break;

                    case btnMid:
                        Brightness = BRT_MID;
                        Led.SetColor(clDarkYellow);
                        break;

                    case btnFull:
                        Brightness = BRT_FULL;
                        Led.SetColor(clDarkRed);
                        break;
                } // switch btn
            } // while get info
        } // if buttons

        if(EvtMsk & EVTMSK_NEW_BRT) {
            Output.Set(Brightness);
        }
    } // while true
} // Main thread
