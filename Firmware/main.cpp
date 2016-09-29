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
#include "kl_adc.h"

App_t App;
LedBlinker_t Led[CHNL_CNT] = { {LED0_PIN}, {LED1_PIN}, {LED2_PIN}, {LED3_PIN} };
static TmrKL_t TmrEverySecond {MS2ST(1000), EVT_EVERY_SECOND, tktPeriodic};
static TmrKL_t TmrAdc {MS2ST(450), EVT_SAMPLING, tktPeriodic};

#define R2Time(Radc)    ((((TIME_MAX_S - TIME_MIN_S) * Radc) / ADC_MAX_VALUE) + TIME_MIN_S)

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

    // LEDs
    for(int i=0; i<CHNL_CNT; i++) {
        Led[i].Init();
        Led[i].StartOrRestart(lsqStart);
        chThdSleepMilliseconds(90);
    }

    // Buttons
    PinSensors.Init();

    // Potentiometers
    PinSetupAnalog(R0_PIN);
    PinSetupAnalog(R1_PIN);
    PinSetupAnalog(R2_PIN);
    PinSetupAnalog(R3_PIN);
    Adc.Init();
    Adc.EnableVref();

    // Timers
    TmrEverySecond.InitAndStart();
    TmrAdc.InitAndStart();

    Radio.Init();

    // Main cycle
    App.ITask();
}

__noreturn
void App_t::ITask() {
    while(true) {
        __unused eventmask_t Evt = chEvtWaitAny(ALL_EVENTS);
        if(Evt & EVT_EVERY_SECOND) {
            // Check if time to switch lamp off
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

        if(Evt & EVT_SAMPLING) {
            Adc.StartMeasurement();
        }

        if(Evt & EVT_ADC_DONE) {
            for(int i=0; i<CHNL_CNT; i++) {
                uint32_t Vr = Adc.GetResult(AdcChannels[i]);
                uint32_t t = R2Time(Vr);
//                Uart.Printf("%u: %u => %u\r", i, Vr, t);
                if(State[i] == stIdle) TimeLeft_s[i] = t;
//                Uart.Printf("%u\r", TimeLeft_s[i]);
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

