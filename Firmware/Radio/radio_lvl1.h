/*
 * radio_lvl1.h
 *
 *  Created on: Nov 17, 2013
 *      Author: kreyl
 */

#ifndef RADIO_LVL1_H_
#define RADIO_LVL1_H_

#include <kl_lib.h>
#include "ch.h"
#include "rlvl1_defins.h"
#include "cc1101.h"
#include "kl_buf.h"

#define TESTWORD      0xCA115EA1

class rLevel1_t {
private:
    rPkt_t Pkt;
public:
    uint8_t Init();
    // Inner use
    void ITask();
    rLevel1_t(): Pkt({0,0}) {}
};

extern rLevel1_t Radio;

#endif /* RADIO_LVL1_H_ */
