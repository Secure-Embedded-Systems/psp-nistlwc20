#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include "cells.h"
#include <string.h>

void transpose64(MDTYPE data[32][2], MDTYPE *t_data) {
    int i, j;
    for (i = 0; i < 64; i++) {
        t_data[i] = 0;
        for (j = 0; j < 32; j++) {
            t_data[i] |= ((data[j][1 - (i / 32)] & (1 << (i % 32))) >> (i % 32)) << j;
        }
    }
}

void inv_transpose64(MDTYPE *t_data, MDTYPE data[32][2]) {
    int i, j;
    for (i = 0; i < 32; i++) {
        data[i][0] = 0;
        data[i][1] = 0;
        data[i][2] = 0;
        data[i][3] = 0;
        for (j = 0; j < 64; j++) {
            data[i][1 - (j / 32)] |= ((t_data[j] & (1 << i)) >> i) << (j % 32);
        }
    }
}

#define MDTYPE_ONE  0xFFFFFFFF
#define MDTYPE_ZERO 0x0

unsigned tick = 0;
#define TICK(A) printf("<%d> %s\n", tick++, #A)

void main() {

    MDTYPE clk = 0;
    MDTYPE reset = 0;
    MDTYPE imode = 0;
    MDTYPE idomsep[2];
    MDTYPE ivalid = 0;
    MDTYPE idata[32][2];
    MDTYPE t_idata[64];
    MDTYPE ipadding = 0;
    MDTYPE ovalid = 0;
    MDTYPE odata[32][2];
    MDTYPE t_odata[64];
    MDTYPE oready = 0;

    memset(idomsep, 0, 2*sizeof(MDTYPE));
    memset(t_idata,  0, 64*sizeof(MDTYPE));
    memset(t_odata,  0, 64*sizeof(MDTYPE));

    reset = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(reset);
    reset = MDTYPE_ZERO;

    // nonce
    idomsep[0]  = MDTYPE_ZERO;
    idomsep[1]  = MDTYPE_ZERO;
    ipadding    = MDTYPE_ZERO;
    ivalid      = MDTYPE_ONE;
    idata[0][0] = 0x0;
    idata[0][1] = 0x01000010;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0x44000440;
    idata[0][1] = 0x04800048;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0x22000230;
    idata[0][1] = 0x13400034;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0x56000560;
    idata[0][1] = 0x02000022;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0xDC0005D0;
    idata[0][1] = 0x00000000;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0x08000080;
    idata[0][1] = 0x08800088;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0x24000240;
    idata[0][1] = 0x04400046;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0x9A0001A2;
    idata[0][1] = 0x2AC000AC;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    idata[0][0] = 0x1000000C;
    idata[0][1] = 0x1B8000BA;
    transpose64(idata, t_idata);
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(nonce);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);

    // key
    idata[0][0] = 0x00111122;
    idata[0][1] = 0x335588DD;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(key);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);

    // key
    idata[0][0] = 0x00111122;
    idata[0][1] = 0x335588DD;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(key);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);

    // AD
    idomsep[0]  = MDTYPE_ZERO;
    idomsep[1]  = MDTYPE_ONE;
    idata[0][0] = 0x11223355;
    idata[0][1] = 0x88DD0011;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(AD);


    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);

/*//comment for 8B AD
    idomsep[0]  = MDTYPE_ZERO;
    idomsep[1]  = MDTYPE_ONE;
    idata[0][0] = 0x11223355;
    idata[0][1] = 0x88DD0080;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(AD);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);
*/
    // encryption
/*//comment for 8B PT
    idomsep[0]  = MDTYPE_ONE;
    idomsep[1]  = MDTYPE_ZERO;
    idata[0][0] = 0x335588DD;
    idata[0][1] = 0x00111122;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(AD);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);
*/
    idomsep[0]  = MDTYPE_ONE;
    idomsep[1]  = MDTYPE_ZERO;
    idata[0][0] = 0x335588DD;
    idata[0][1] = 0x00111180;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(AD);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);

    // finalization
    idomsep[0]  = MDTYPE_ZERO;
    idomsep[1]  = MDTYPE_ZERO;
    idata[0][0] = 0x00111122;
    idata[0][1] = 0x335588DD;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(AD);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        //TICK(crunch);
    } while (oready != MDTYPE_ONE);

    idomsep[0]  = MDTYPE_ZERO;
    idomsep[1]  = MDTYPE_ZERO;
    idata[0][0] = 0x00111122;
    idata[0][1] = 0x335588DD;
    transpose64(idata, t_idata);
    ivalid   = MDTYPE_ONE;
    wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
    //TICK(AD);

    ivalid   = MDTYPE_ZERO;
    do {
        wage(clk, reset, imode, idomsep, ivalid, t_idata, ipadding, &ovalid, &oready, t_odata);
        ////TICK(crunch);
        if (ovalid == MDTYPE_ONE) {
            inv_transpose64(t_odata, odata);
            //printf("Cipher Out %8x %8x\n", odata[0][0], odata[0][1]);
        }
    } while (oready != MDTYPE_ONE);


}
