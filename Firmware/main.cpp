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

App_t App;

LedBlinker_t Led[4] = { {LED0_PIN}, {LED1_PIN}, {LED2_PIN}, {LED3_PIN} };

int main(void) {
    // ==== Init Vcore & clock system ====
    SetupVCore(vcore1V5);
    Clk.UpdateFreqValues();

    // Init OS
    halInit();
    chSysInit();

    App.InitThread();

    // ==== Init hardware ====
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();

    for(int i=0; i<4; i++) {
        Led[i].Init();
        Led[i].StartOrRestart(lsqStart);
        chThdSleepMilliseconds(180);
    }

    PinSensors.Init();

    Radio.Init();

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
                    Uart.Printf("Btn %u\r", EInfo.BtnID);
                    uint32_t indx = EInfo.BtnID;
                    if(State[indx] == stIdle) {
                        Led[indx].StartOrContinue(lsqWaitReply);
                        State[indx] = stWaitingReply;
                    }
                    else if(State[indx] == stWaitingReply) {
                        Led[indx].Stop();
                        State[indx] = stIdle;
                    }
                } // if press
            } // while
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

