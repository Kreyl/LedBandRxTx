/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "main.h"
#include "board.h"
#include "radio_lvl1.h"
#include "led.h"
#include "color.h"

#define REMCTRL_ID  8   // ID of remote control

enum AppState_t { appsIdle = 0, appsRed = 1, appsBlue = 2, appsWhite = 3 };

App_t App;
LedRGB_t Led { LED_RED_CH, LED_GREEN_CH, LED_BLUE_CH };

int main(void) {
    // ==== Init Vcore & clock system ====
    SetupVCore(vcore1V5);
//    Clk.SetMSI4MHz();
//    Clk.SetupFlashLatency(16);
//    Clk.SwitchToHSI();
    Clk.UpdateFreqValues();

    // Init OS
    halInit();
    chSysInit();

    App.InitThread();

    // ==== Init hardware ====
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();

    Led.Init();
    Led.SetColor(clGreen);

    // Init radio
//    bool RadioIsOk = false;
//    for(int i=0; (i<7 and !RadioIsOk); i++) {
//        if(Radio.Init() == OK) RadioIsOk = true;
//    }
//    if(RadioIsOk) {
//        Effects.AllTogetherSmoothly(clGreen, 180);
//        chThdSleepMilliseconds(900);
//        Effects.AllTogetherSmoothly(clBlack, 180);
//    }
//    else {
//        Effects.AllTogetherNow(clRed);
//        chThdSleepMilliseconds(180);
//        Effects.AllTogetherNow(clBlack);
//        chThdSleepMilliseconds(180);
//        Effects.AllTogetherNow(clRed);
//        chThdSleepMilliseconds(180);
//        Effects.AllTogetherNow(clBlack);
//        chThdSleepMilliseconds(180);
//        Effects.AllTogetherNow(clRed);
//        chThdSleepMilliseconds(180);
//        Effects.AllTogetherNow(clBlack);
//    }

    // Main cycle
    App.ITask();
}

__noreturn
void App_t::ITask() {
    while(true) {
        __unused eventmask_t Evt = chEvtWaitAny(ALL_EVENTS);

        if(Evt & EVT_RADIO) {
        } // Evt

#if UART_RX_ENABLED
        if(Evt & EVT_UART_NEW_CMD) {
            OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }
#endif

    } // while true
}

#if UART_RX_ENABLED // ======================= Command processing ============================
void App_t::OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(OK);
    }

    else PShell->Ack(CMD_UNKNOWN);
}
#endif

