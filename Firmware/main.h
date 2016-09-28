/*
 * main.h
 *
 *  Created on: 15 сент. 2014 г.
 *      Author: g.kruglov
 */

#pragma once

#include "ch.h"
#include "kl_lib.h"
#include "uart.h"
#include "evt_mask.h"
#include "board.h"

// Color to transmit
#define COLOR_TX_R  180
#define COLOR_TX_G  180
#define COLOR_TX_B  0

#define CHNL_CNT    4

enum ChannelState_t { stIdle = 0, stWaitingReply, stActivated };

class App_t {
private:
    thread_t *PThread;
public:
    ChannelState_t State[CHNL_CNT];
    uint32_t TimeLeft_s[CHNL_CNT];
    // Eternal methods
    void InitThread() { PThread = chThdGetSelfX(); }
    uint8_t GetDipSwitch();
    void SignalEvt(eventmask_t Evt) {
        chSysLock();
        chEvtSignalI(PThread, Evt);
        chSysUnlock();
    }
    void SignalEvtI(eventmask_t Evt) { chEvtSignalI(PThread, Evt); }
    void OnCmd(Shell_t *PShell);
    // Inner use
    void ITask();
};

extern App_t App;
