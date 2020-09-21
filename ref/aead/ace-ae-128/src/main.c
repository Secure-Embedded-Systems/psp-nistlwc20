#include "msp.h"
#include <stdio.h>
#include <string.h>

#include "api.h"
#include "crypto_aead.h"

/**
 * main.c
 */


int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    int result = 0;
    unsigned long long alen = 0;
    unsigned long long mlen = 0;
    unsigned long long clen = CRYPTO_ABYTES;
    unsigned char a[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    alen = 12; //0
    unsigned char m[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    mlen = 16;
    int clength = strlen((const char*)m) + CRYPTO_ABYTES;
    unsigned char c[21];
    //unsigned char nsec[CRYPTO_NSECBYTES]; nsec = NULL
    unsigned char npub[CRYPTO_NPUBBYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    unsigned char k[CRYPTO_KEYBYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};


//    volatile int i = 0;

//    for(i = 0; i<100; i++)
        result |= crypto_aead_encrypt(c, &clen, m, mlen, a, alen, NULL, npub, k);


    return result;
}
