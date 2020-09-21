//#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "cells.h"



void transpose32(unsigned in, unsigned out[32]) {
  unsigned bit, word;
  for (word = 0; word < 32; word++){
      out[word] = 0;
  }
  for (bit = 0; bit < 32; bit++) {
      if ((in & (1<<bit)) == 0) {
          out[bit] = 0;
      } else {
          out[bit] = 0xffffffff;
      }
  }

}

unsigned int transpose32_reverse(unsigned in[32]) {
  unsigned bit, word;
  unsigned out = 0;
  for (word = 0; word < 32; word++) {
      if (in[word] == 0xffffffff) {
          out = out | (1<<word);
      }
  }
  return out;
}



int main(void) {



    /**** ascon ****/
    MDTYPE clk = -1;
    MDTYPE rst = 0;
    MDTYPE npub [128];
    MDTYPE key [128];
    MDTYPE bdi [64];
    MDTYPE exp_tag [128];
    MDTYPE key_ready = 0;
    MDTYPE key_updated; // output
    MDTYPE key_needs_update = 0;
    MDTYPE npub_ready = 0;
    MDTYPE npub_read; // output
    MDTYPE bdi_ready = 0;
    MDTYPE bdi_proc = 0;
    MDTYPE bdi_ad = 0;
    MDTYPE bdi_wait; // output
    MDTYPE bdi_decrypt = 0;
    MDTYPE bdi_eot = 0;
    MDTYPE bdi_eoi = 0;
    MDTYPE bdi_read; // output
    MDTYPE bdi_size [3];
    MDTYPE exp_tag_ready = 0;
    MDTYPE bdo_ready = 0;
    MDTYPE bdo_write; // output
    MDTYPE bdo [64]; // output
    MDTYPE bdo_size [4]; // output
    MDTYPE tag_ready = 0;
    MDTYPE tag_write; // output
    MDTYPE tag [128]; // output
    MDTYPE msg_auth_done; // output
    MDTYPE msg_auth_valid; // output

    int i;


    for (i=0 ; i<3 ; i++){
        bdi_size[i] = 0;
    }
    for (i=0 ; i<64 ; i++){
        bdi[i] = 0;
    }
    for (i=0 ; i<128 ; i++){
        npub[i] = 0;
        key[i] = 0;
        exp_tag[i] = 0;
    }

    //Key transpose
    MDTYPE key_ref [4];
    key_ref[0] = 0x55555555;
    transpose32(key_ref[0], &key[0]);
    key_ref[1] = 0x55555555;
    transpose32(key_ref[1], &key[32]);
    key_ref[2] = 0x55555555;
    transpose32(key_ref[2], &key[64]);
    key_ref[3] = 0x55555555;
    transpose32(key_ref[3], &key[96]);

    //Nonce transpose
    MDTYPE npub_ref [4];
    npub_ref[0] = 0xB0B0B0B0;
    transpose32(npub_ref[0], &npub[0]);
    npub_ref[1] = 0xB0B0B0B0;
    transpose32(npub_ref[1], &npub[32]);
    npub_ref[2] = 0xB0B0B0B0;
    transpose32(npub_ref[2], &npub[64]);
    npub_ref[3] = 0xB0B0B0B0;
    transpose32(npub_ref[3], &npub[96]);

    //AD transpose
    MDTYPE ad_ref [3];
    ad_ref[0] = 0xA0A0A0A0;
    transpose32(ad_ref[0], &bdi[0]);
    ad_ref[1] = 0xA0A0A0A0;
    transpose32(ad_ref[1], &bdi[32]);
//    ad_ref[2] = 0xA0A0A0A0;
//    transpose32(ad_ref[2], &bdi[32]);



    // reset:
    rst = -1;
    ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
          &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
          &tag_write, tag, &msg_auth_done, &msg_auth_valid);
    rst = 0; // undo reset


    // send key:
    key_ready = -1;
    key_needs_update = -1;
    ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
          &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
          &tag_write, tag, &msg_auth_done, &msg_auth_valid);

    key_ready = 0; // undo
    key_needs_update = 0; // undo

    // send npub
    npub_ready = -1;
    bdi_proc = -1;
    bdi_decrypt = 0; // for encryption
    while (bdi_wait==0) { // wait for bdi_wait for starting next transmission
        ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
              &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
              &tag_write, tag, &msg_auth_done, &msg_auth_valid);
    }
    npub_ready = 0; // undo
    bdi_proc = 0; // undo

    // send ad
    bdi_ready = -1;
    bdi_ad = -1;
    bdi_eoi = 0; // we have pt
    bdi_eot = -1; // last ad block
    // full size:
    bdi_size[0] = 0;
    bdi_size[1] = 0;
    bdi_size[2] = 0;
    while (bdi_read==0) { // wait for bdi_read
        ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
              &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
              &tag_write, tag, &msg_auth_done, &msg_auth_valid);
    }

    bdi_ready = 0;
    bdi_ad = 0;
//    bdi_eoi = 0;
    bdi_eot = 0; // ?
//    bdi_size[0] = 0;
//    bdi_size[1] = 0;
//    bdi_size[2] = 0;

    while (bdi_wait==0) {
        ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
              &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
              &tag_write, tag, &msg_auth_done, &msg_auth_valid);
    }



    // send pt


    //PT transpose
    MDTYPE pt_ref [2];
    pt_ref[0] = 0xFFFFFFFF;
    transpose32(pt_ref[0], &bdi[0]);
    pt_ref[1] = 0xFFFFFFFF;
    transpose32(pt_ref[1], &bdi[32]);

    bdi_ready = -1;
    bdi_ad = 0;
    bdi_eoi = -1; // last bdi
    bdi_eot = -1; // last pt block
    // full size:
    bdi_size[0] = 0;
    bdi_size[1] = 0;
    bdi_size[2] = 0;
    while (bdi_read==0) { // wait for bdi_read
        ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
              &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
              &tag_write, tag, &msg_auth_done, &msg_auth_valid);
    }
    bdi_ready = 0;
    bdi_ad = 0;
    bdi_eoi = 0;
    bdi_eot = 0; // ?

    // send bdo_ready and read ciphertext
    bdo_ready = -1;
    while (bdo_write==0) { // wait for bdo_write
        ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
              &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
              &tag_write, tag, &msg_auth_done, &msg_auth_valid);
    }
    bdo_ready = 0; //undo

    // send tag_ready and read tag
    tag_ready = -1;
    while (tag_write==0) { // wait for tag_write
        ascon(clk, rst, npub, key, bdi, exp_tag, key_ready, &key_updated, key_needs_update, npub_ready, &npub_read, bdi_ready, bdi_proc, bdi_ad,
              &bdi_wait, bdi_decrypt, bdi_eot, bdi_eoi, &bdi_read, bdi_size, exp_tag_ready, bdo_ready, &bdo_write, bdo, bdo_size, tag_ready,
              &tag_write, tag, &msg_auth_done, &msg_auth_valid);
    }

    printf("\n Finish");
    tag_ready++;
    bdo_ready = tag_ready;
    return 0;

}
