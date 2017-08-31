/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "board.h"
#include "shell.h"
#include "MsgQ.h"
#include "radio_lvl1.h"
#include "led.h"
#include "Sequences.h"
#include "SimpleSensors.h"
#include "buttons.h"

// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
extern CmdUart_t Uart;
static void ITask();
static void OnCmd(Shell_t *PShell);

extern cc1101_t CC;
LedRGB_t Led {LED_RED_CH, LED_GREEN_CH, LED_BLUE_CH};

int main(void) {
    // ==== Init Vcore & clock system ====
    SetupVCore(vcore1V5);
    Clk.EnablePrefetch();
    Clk.UpdateFreqValues();

    // Init OS
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init(115200);
    Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();

    // Buttons
    SimpleSensors::Init();

    Led.Init();
    Led.StartOrRestart(lsqStart);

    Radio.Init();

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {

            case evtIdButtons: {
                Printf("Btn %u\r", Msg.BtnEvtInfo.BtnID);
                // Try to transmit and receive ack
                bool AckRcvd = false;
                for(int i=0; i<7; i++) {
                    Radio.Pkt.Btn = Msg.BtnEvtInfo.BtnID;
                    Radio.Pkt.TestWord = TEST_WORD;
                    CC.Recalibrate();
                    CC.Transmit(&Radio.Pkt);
                    uint8_t RxRslt = CC.Receive(54, &Radio.Pkt, &Radio.Rssi);
                    if(RxRslt == retvOk and Radio.Pkt.TestWord == TEST_WORD) {
                        AckRcvd = true;
                        break;
                    }
                } // for
                if(AckRcvd) Led.StartOrRestart(lsqSuccess);
                else Led.StartOrRestart(lsqFailure);
                } break;

            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;
            default: Printf("Unhandled Msg %u\r", Msg.ID); break;
        } // Switch
    } // while true
}

#if UART_RX_ENABLED // ================= Command processing ====================
void OnCmd(Shell_t *PShell) {
    Cmd_t *PCmd = &PShell->Cmd;
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }

    else if(PCmd->NameIs("Version")) PShell->Printf("%S %S\r", APP_NAME, BUILD_TIME);

    else PShell->Ack(retvCmdUnknown);
}
#endif
