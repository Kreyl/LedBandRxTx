/*
 * radio_lvl1.cpp
 *
 *  Created on: Nov 17, 2013
 *      Author: kreyl
 */

#include "radio_lvl1.h"
#include "evt_mask.h"
#include "main.h"
#include "cc1101.h"
#include "uart.h"

#define DBG_PINS

#ifdef DBG_PINS
#define DBG_GPIO1   GPIOB
#define DBG_PIN1    0
#define DBG1_SET()  PinSetHi(DBG_GPIO1, DBG_PIN1)
#define DBG1_CLR()  PinSetLo(DBG_GPIO1, DBG_PIN1)
//#define DBG_GPIO2   GPIOB
//#define DBG_PIN2    9
//#define DBG2_SET()  PinSet(DBG_GPIO2, DBG_PIN2)
//#define DBG2_CLR()  PinClear(DBG_GPIO2, DBG_PIN2)
#else
#define DBG1_SET()
#define DBG1_CLR()
#endif

rLevel1_t Radio;

#if 1 // ================================ Task =================================
static THD_WORKING_AREA(warLvl1Thread, 256);
__noreturn
static void rLvl1Thread(void *arg) {
    chRegSetThreadName("rLvl1");
    while(true) {
        // Send color or send "black" depending on switch
        if(App.SendColorIndication == true) {
            Radio.PktTx.R = COLOR_TX_R;
            Radio.PktTx.G = COLOR_TX_G;
            Radio.PktTx.B = COLOR_TX_B;
        }
        else {
            Radio.PktTx.R = 0;
            Radio.PktTx.G = 0;
            Radio.PktTx.B = 0;
        }

        // Iterate channels and send command if required
        for(int i=0; i<CHNL_CNT; i++) {
            if(App.State[i] == stWaitingReply) {
                CC.SetChannel(ID2RCHNL(i+1));
                Radio.PktTx.TimeLeft_s = App.TimeLeft_s[i];
                CC.Transmit(&Radio.PktTx);  // Send data
                // Wait ack
                uint8_t RxRslt = CC.Receive(54, &Radio.PktRx, &Radio.Rssi);
                if(RxRslt == OK) {
                    Uart.Printf("%d: Rssi=%d\r", i, Radio.Rssi);
                    if(Radio.PktRx.TheWord == THE_WORD) {
                        App.State[i] = stActivated;
                        App.SignalEvt(EVT_RADIO);
                    }
                }
            }
        } // for
        chThdSleepMilliseconds(90);
    } // while true
}
#endif // task

#if 1 // ============================
uint8_t rLevel1_t::Init() {
#ifdef DBG_PINS
    PinSetupOut(DBG_GPIO1, DBG_PIN1, omPushPull);
//    PinSetupOut(DBG_GPIO2, DBG_PIN2, omPushPull);
#endif
    if(CC.Init() == OK) {
        CC.SetTxPower(CC_Pwr0dBm);
        CC.SetPktSize(RPKT_LEN);
//        CC.SetChannel(ID2RCHNL(App.ID));
//        CC.EnterPwrDown();
        // Thread
        PThd = chThdCreateStatic(warLvl1Thread, sizeof(warLvl1Thread), HIGHPRIO, (tfunc_t)rLvl1Thread, NULL);
        return OK;
    }
    else return FAILURE;
}
#endif
