/*
 * main.c
 *
 *  Created on: Sep 15, 2020
 *      Author: Archanaa
 */


#include "msp.h"
#include "msp432.h"
#include <stdio.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned long long u64;

typedef struct {
  u64 x0, x1, x2, x3, x4;
} state;
//#include "permutations.h"
static inline u64 ROTR64(u64 x, int n) {
    return (x << (64 - n)) | (x >> n);
}

static inline void ROUND(u8 C, state* p) {
  state s = *p;
  state t;
  // addition of round constant
  s.x2 ^= C;
  // substitution layer
  s.x0 ^= s.x4;
  s.x4 ^= s.x3;
  s.x2 ^= s.x1;
  // start of keccak s-box
  t.x0 = ~s.x0;
  t.x1 = ~s.x1;
  t.x2 = ~s.x2;
  t.x3 = ~s.x3;
  t.x4 = ~s.x4;
  t.x0 &= s.x1;
  t.x1 &= s.x2;
  t.x2 &= s.x3;
  t.x3 &= s.x4;
  t.x4 &= s.x0;
  s.x0 ^= t.x1;
  s.x1 ^= t.x2;
  s.x2 ^= t.x3;
  s.x3 ^= t.x4;
  s.x4 ^= t.x0;
  // end of keccak s-box
  s.x1 ^= s.x0;
  s.x0 ^= s.x4;
  s.x3 ^= s.x2;
  s.x2 = ~s.x2;
  // linear diffusion layer
  s.x0 ^= ROTR64(s.x0, 19) ^ ROTR64(s.x0, 28);
  s.x1 ^= ROTR64(s.x1, 61) ^ ROTR64(s.x1, 39);
  s.x2 ^= ROTR64(s.x2, 1) ^ ROTR64(s.x2, 6);
  s.x3 ^= ROTR64(s.x3, 10) ^ ROTR64(s.x3, 17);
  s.x4 ^= ROTR64(s.x4, 7) ^ ROTR64(s.x4, 41);
  *p = s;
}

static inline void P12(state* s) {
  ROUND(0xf0, s);
  ROUND(0xe1, s);
  ROUND(0xd2, s);
  ROUND(0xc3, s);
  ROUND(0xb4, s);
  ROUND(0xa5, s);
  ROUND(0x96, s);
  ROUND(0x87, s);
  ROUND(0x78, s);
  ROUND(0x69, s);
  ROUND(0x5a, s);
  ROUND(0x4b, s);
}

int main() {

  state input = { 0 };
//  volatile int i = 0;
//  for ( i=0; i<100; i++) {
//    printf("CIPHR %16lx %16lx %16lx %16lx %16lx\n", input.x0, input.x1, input.x2, input.x3, input.x4);
    P12(&input);
//  }

  return 0;
}
