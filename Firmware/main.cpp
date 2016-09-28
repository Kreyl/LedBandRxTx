/*
 * main.cpp
 *
 *  Created on: 20 ����. 2014 �.
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
LedBlinker_t Led[CHNL_CNT] = { {LED0_PIN}, {LED1_PIN}, {LED2_PIN}, {LED3_PIN} };
static TmrKL_t TmrEverySecond {MS2ST(1000), EVT_EVERY_SECOND, tktPeriodic};

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

    for(int i=0; i<CHNL_CNT; i++) {
        Led[i].Init();
        Led[i].StartOrRestart(lsqStart);
        chThdSleepMilliseconds(90);
    }
    PinSensors.Init();

    TmrEverySecond.InitAndStart();

    Radio.Init();

    // Main cycle
    App.ITask();
}

__noreturn
void App_t::ITask() {
    while(true) {
        __unused eventmask_t Evt = chEvtWaitAny(ALL_EVENTS);
        if(Evt & EVT_EVERY_SECOND) {
            for(int i=0; i<CHNL_CNT; i++) {
                if(State[i] == stActivated) {
                    if(TimeLeft_s[i] > 0) TimeLeft_s[i]--;
                    else {
                        Led[i].Stop();
                        State[i] = stIdle;
                    }
                } // if activated
            } // for
        }

        if(Evt & EVT_BUTTONS) {
            BtnEvtInfo_t EInfo;
            while(BtnGetEvt(&EInfo) == OK) {
                if(EInfo.Type == bePress) {
                    Uart.Printf("Btn %u\r", EInfo.BtnID);
                    uint32_t indx = EInfo.BtnID;
                    if(indx < CHNL_CNT) {
                        if(State[indx] == stIdle) {
                            Led[indx].StartOrContinue(lsqWaitReply);
                            TimeLeft_s[indx] = 4; // TODO
                            State[indx] = stWaitingReply;
                        }
                        else if(State[indx] == stWaitingReply) {
                            Led[indx].Stop();
                            State[indx] = stIdle;
                        }
                    }
                } // if press
            } // while
        } // Evt

        if(Evt & EVT_RADIO) { // Ack received
            // Check states
            for(int i=0; i<CHNL_CNT; i++) {
                if(State[i] == stActivated) {
                    Led[i].StartOrContinue(lsqActivated);
                }
            }
        }

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

