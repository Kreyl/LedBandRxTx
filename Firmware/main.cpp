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
#include "Sequences.h"
#include "SimpleSensors.h"
#include "buttons.h"

AppState_t appState = appsStandby;

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
    PinSensors.Init();

    // Init radio
    bool RadioIsOk = false;
    for(int i=0; (i<7 and !RadioIsOk); i++) {
        if(Radio.Init() == OK) RadioIsOk = true;
    }
    if(RadioIsOk) Led.SetColor((Color_t){0, 1, 0});
    else Led.StartSequence(lsqFailure);
    chThdSleepMilliseconds(900);

    // Main cycle
    App.ITask();
}

__noreturn
void App_t::ITask() {
    while(true) {
        __unused eventmask_t Evt = chEvtWaitAny(ALL_EVENTS);

        if(Evt & EVT_BUTTONS) {
            BtnEvtInfo_t EInfo;
            while(BtnGetEvt(&EInfo) == OK) {
                if(EInfo.Type == bePress) {
//                    Uart.Printf("Btn %u\r", EInfo.BtnID);
                    // Switch state
                    switch((BtnName_t)EInfo.BtnID) {
                        case btnRed:
                            if(appState == appsRed) appState = appsStandby;
                            else appState = appsRed;
                            break;
                        case btnBlue:
                            if(appState == appsBlue) appState = appsStandby;
                            else appState = appsBlue;
                            break;
                        case btnWhite:
                            if(appState == appsWhite) appState = appsStandby;
                            else appState = appsWhite;
                            break;
                        case btnOff:
                            if(appState == appsIdle) appState = appsStandby;
                            else appState = appsIdle;
                            break;
                    } // switch
                    // Process new state
                    Radio.MustTx = (appState != appsStandby);
                    // Indicate
#define IND_BRT     45
                    switch(appState) {
                        case appsRed:     Led.SetColor((Color_t){IND_BRT, 0, 0}); break;
                        case appsBlue:    Led.SetColor((Color_t){0, 0, IND_BRT}); break;
                        case appsWhite:   Led.SetColor((Color_t){IND_BRT, IND_BRT, IND_BRT}); break;
                        case appsIdle:    Led.SetColor((Color_t){IND_BRT, IND_BRT, 0}); break;
                        case appsStandby: Led.SetColor((Color_t){0, 1, 0}); break;
                    }
                } // if press
            }
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

