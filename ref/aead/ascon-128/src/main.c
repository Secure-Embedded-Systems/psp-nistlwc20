#include "msp.h"
#include <stdio.h>
#include <string.h>

#include "api.h"

/**
 * main.c
 */
int crypto_aead_encrypt(
    unsigned char *c, unsigned long long *clen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k
);

int crypto_aead_decrypt(
    unsigned char *m, unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k
);

void print(unsigned char c, unsigned char* x, unsigned long long xlen) {
  unsigned long long i;
  printf("%c[%d]=", c, (int)xlen);
  for (i = 0; i < xlen; ++i) printf("%02x", x[i]);
  printf("\n");
}

/*
 * Test vector from https://github.com/ascon/ascon-c/blob/2131f22a1d6bc0c065cfbf5821c7a4380e2e7d85/crypto_aead/ascon128v12/LWC_AEAD_KAT_128_128.txt
 * Count = 1
Key = 000102030405060708090A0B0C0D0E0F
Nonce = 000102030405060708090A0B0C0D0E0F
PT =
AD =
CT = E355159F292911F794CB1432A0103A8A
 */
int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    int result = 0;
    unsigned long long alen = 0;
    unsigned long long mlen = 0;
    unsigned long long clen = CRYPTO_ABYTES;
    //unsigned char a[] = "ASCON";
    unsigned char a[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    alen = 8; //0
    //unsigned char m[] = "ascon";
    unsigned char m[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    mlen = 8;
    int clength = strlen((const char*)m) + CRYPTO_ABYTES;
    unsigned char c[21];
    //unsigned char nsec[CRYPTO_NSECBYTES]; nsec = NULL
    unsigned char npub[CRYPTO_NPUBBYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    unsigned char k[CRYPTO_KEYBYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x013};

//    volatile int i = 0;

//    for(i = 0; i<100; i++)
        result |= crypto_aead_encrypt(c, &clen, m, mlen, a, alen, NULL, npub, k);


    return result;
}
