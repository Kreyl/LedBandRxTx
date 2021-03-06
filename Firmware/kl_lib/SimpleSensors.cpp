/*
 * SimpleSensors.cpp
 *
 *  Created on: 17 ���. 2015 �.
 *      Author: Kreyl
 */

#include "SimpleSensors.h"

SimpleSensors_t PinSensors;

// ==== Sensors Thread ====
static WORKING_AREA(waPinSnsThread, 128);
__attribute__((noreturn))
static void SensorsThread(void *arg) {
    chRegSetThreadName("PinSensors");
    PinSensors.ITask();
}

void SimpleSensors_t::Init() {
    // Init pins
    for(uint32_t i=0; i < PIN_SNS_CNT; i++) {
        PinSns[i].Init();
        States[i] = pssLo;
    }
    // Create and start thread
    chThdCreateStatic(waPinSnsThread, sizeof(waPinSnsThread), NORMALPRIO, (tfunc_t)SensorsThread, NULL);
}

__attribute__((noreturn))
void SimpleSensors_t::ITask() {
    while(true) {
        chThdSleepMilliseconds(SNS_POLL_PERIOD_MS);
        ftVoidPSnsStLen PostProcessor = PinSns[0].Postprocessor;
        uint32_t GroupLen = 0;
        PinSnsState_t *PStates = &States[0];
        // ==== Iterate pins ====
        uint32_t i=0;
        while(i < PIN_SNS_CNT) {
            // Check pin
            if(PinSns[i].IsHi()) {
                if(States[i] == pssLo) States[i] = pssRising;
                else States[i] = pssHi;
            }
            else { // is low
                if(States[i] == pssHi) States[i] = pssFalling;
                else States[i] = pssLo;
            }
            GroupLen++;
            // Call postprocessor if this was last pin in group (or last at all)
            i++;
            if((i >= PIN_SNS_CNT) or (PinSns[i].Postprocessor != PostProcessor)) {
                if(PostProcessor != nullptr) PostProcessor(PStates, GroupLen);
                // Prepare for next group
                PostProcessor = PinSns[i].Postprocessor;
                GroupLen = 0;
                PStates = &States[i];
            }
        } // while i
    } // while true
}
