/* Reference implementation of the WAGE-128 permutation
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdint.h>
#include "wage.h"

/***********************************************************
  ******* WAGE permutation implementation ******************
  **********************************************************/

/*
 *RC0: 7-bit round constants applied on S18
 *RC1: 7-bit round constants applied on S36
 */
const unsigned char RC0[111] = {0x7f,0x1f,0x07,0x01,0x20,0x08,0x02,0x60,0x18,0x06,0x21,0x28,0x0a,0x62,0x78,0x1e,0x27,0x09,0x22,0x68,0x1a,0x66,0x39,0x2e,0x2b,0x4a,0x72,0x7c,0x5f,0x17,0x05,0x61,0x38,0x0e,0x23,0x48,0x12,0x64,0x59,0x36,0x2d,0x6b,0x5a,0x76,0x3d,0x6f,0x1b,0x46,0x31,0x2c,0x4b,0x52,0x74,0x5d,0x77,0x1d,0x67,0x19,0x26,0x29,0x2a,0x6a,0x7a,0x7e,0x3f,0x0f,0x03,0x40,0x10,0x04,0x41,0x30,0x0c,0x43,0x50,0x14,0x45,0x71,0x3c,0x4f,0x13,0x44,0x51,0x34,0x4d,0x73,0x5c,0x57,0x15,0x65,0x79,0x3e,0x2f,0x0b,0x42,0x70,0x1c,0x47,0x11,0x24,0x49,0x32,0x6c,0x5b,0x56,0x35,0x6d,0x7b,0x5e,0x37,0x0d};
const unsigned char RC1[111] = {0x3f,0x0f,0x03,0x40,0x10,0x04,0x41,0x30,0x0c,0x43,0x50,0x14,0x45,0x71,0x3c,0x4f,0x13,0x44,0x51,0x34,0x4d,0x73,0x5c,0x57,0x15,0x65,0x79,0x3e,0x2f,0x0b,0x42,0x70,0x1c,0x47,0x11,0x24,0x49,0x32,0x6c,0x5b,0x56,0x35,0x6d,0x7b,0x5e,0x37,0x0d,0x63,0x58,0x16,0x25,0x69,0x3a,0x6e,0x3b,0x4e,0x33,0x4c,0x53,0x54,0x55,0x75,0x7d,0x7f,0x1f,0x07,0x01,0x20,0x08,0x02,0x60,0x18,0x06,0x21,0x28,0x0a,0x62,0x78,0x1e,0x27,0x09,0x22,0x68,0x1a,0x66,0x39,0x2e,0x2b,0x4a,0x72,0x7c,0x5f,0x17,0x05,0x61,0x38,0x0e,0x23,0x48,0x12,0x64,0x59,0x36,0x2d,0x6b,0x5a,0x76,0x3d,0x6f,0x1b,0x46};

/*
 *sbox_table: 7-bit sbox SB
 */
const unsigned char sbox_table[128] = {
        0x2e, 0x1c, 0x6d, 0x2b, 0x35, 0x07, 0x7f, 0x3b,
        0x28, 0x08, 0x0b, 0x5f, 0x31, 0x11, 0x1b, 0x4d,
        0x6e, 0x54, 0x0d, 0x09, 0x1f, 0x45, 0x75, 0x53,
        0x6a, 0x5d, 0x61, 0x00, 0x04, 0x78, 0x06, 0x1e,
        0x37, 0x6f, 0x2f, 0x49, 0x64, 0x34, 0x7d, 0x19,
        0x39, 0x33, 0x43, 0x57, 0x60, 0x62, 0x13, 0x05,
        0x77, 0x47, 0x4f, 0x4b, 0x1d, 0x2d, 0x24, 0x48,
        0x74, 0x58, 0x25, 0x5e, 0x5a, 0x76, 0x41, 0x42,
        0x27, 0x3e, 0x6c, 0x01, 0x2c, 0x3c, 0x4e, 0x1a,
        0x21, 0x2a, 0x0a, 0x55, 0x3a, 0x38, 0x18, 0x7e,
        0x0c, 0x63, 0x67, 0x56, 0x50, 0x7c, 0x32, 0x7a,
        0x68, 0x02, 0x6b, 0x17, 0x7b, 0x59, 0x71, 0x0f,
        0x30, 0x10, 0x22, 0x3d, 0x40, 0x69, 0x52, 0x14,
        0x36, 0x44, 0x46, 0x03, 0x16, 0x65, 0x66, 0x72,
        0x12, 0x0e, 0x29, 0x4a, 0x4c, 0x70, 0x15, 0x26,
        0x79, 0x51, 0x23, 0x3f, 0x73, 0x5b, 0x20, 0x5c
    };
/*
 *wgp7_table: WG permutation over GF(2^7) WGP
 */
const unsigned char wgp7_table[128] = {
        0x00,0x12,0x0a,0x4b,0x66,0x0c,0x48,0x73,
        0x79,0x3e,0x61,0x51,0x01,0x15,0x17,0x0e,
        0x7e,0x33,0x68,0x36,0x42,0x35,0x37,0x5e,
        0x53,0x4c,0x3f,0x54,0x58,0x6e,0x56,0x2a,
        0x1d,0x25,0x6d,0x65,0x5b,0x71,0x2f,0x20,
        0x06,0x18,0x29,0x3a,0x0d,0x7a,0x6c,0x1b,
        0x19,0x43,0x70,0x41,0x49,0x22,0x77,0x60,
        0x4f,0x45,0x55,0x02,0x63,0x47,0x75,0x2d,
        0x40,0x46,0x7d,0x5c,0x7c,0x59,0x26,0x0b,
        0x09,0x03,0x57,0x5d,0x27,0x78,0x30,0x2e,
        0x44,0x52,0x3b,0x08,0x67,0x2c,0x05,0x6b,
        0x2b,0x1a,0x21,0x38,0x07,0x0f,0x4a,0x11,
        0x50,0x6a,0x28,0x31,0x10,0x4d,0x5f,0x72,
        0x39,0x16,0x5a,0x13,0x04,0x3c,0x34,0x1f,
        0x76,0x1e,0x14,0x23,0x1c,0x32,0x4e,0x7b,
        0x24,0x74,0x7f,0x3d,0x69,0x64,0x62,0x6f
    };
/*
 *omega_table: lookup table for constant omega
 */
const unsigned char omega_table[128] = {
        0x00,0x78,0x01,0x79,0x02,0x7a,0x03,0x7b,
        0x04,0x7c,0x05,0x7d,0x06,0x7e,0x07,0x7f,
        0x08,0x70,0x09,0x71,0x0a,0x72,0x0b,0x73,
        0x0c,0x74,0x0d,0x75,0x0e,0x76,0x0f,0x77,
        0x10,0x68,0x11,0x69,0x12,0x6a,0x13,0x6b,
        0x14,0x6c,0x15,0x6d,0x16,0x6e,0x17,0x6f,
        0x18,0x60,0x19,0x61,0x1a,0x62,0x1b,0x63,
        0x1c,0x64,0x1d,0x65,0x1e,0x66,0x1f,0x67,
        0x20,0x58,0x21,0x59,0x22,0x5a,0x23,0x5b,
        0x24,0x5c,0x25,0x5d,0x26,0x5e,0x27,0x5f,
        0x28,0x50,0x29,0x51,0x2a,0x52,0x2b,0x53,
        0x2c,0x54,0x2d,0x55,0x2e,0x56,0x2f,0x57,
        0x30,0x48,0x31,0x49,0x32,0x4a,0x33,0x4b,
        0x34,0x4c,0x35,0x4d,0x36,0x4e,0x37,0x4f,
        0x38,0x40,0x39,0x41,0x3a,0x42,0x3b,0x43,
        0x3c,0x44,0x3d,0x45,0x3e,0x46,0x3f,0x47
    };


/*
 *wage_permutation: 111-round wage permutation of width 259 bits
 *state: 259-bit input, and output is stored in state (inplace)
*/
void wage_permutation( unsigned char *state )
{
    unsigned char i, j, fb_end;

    for ( i = 0; i < WAGEROUNDS; i++ )
    {

        //Left half update
        fb_end = omega_table[state[0]]^state[6]^state[8]^state[12]^state[13]^state[19]^state[24]^state[26]^state[30]^state[31]^wgp7_table[state[36]]^RC1[i];
        state[30] = state[30]^sbox_table[state[34]];
        state[24] = state[24]^sbox_table[state[27]];

        //Right half update
        state[19] = state[19]^wgp7_table[state[18]]^RC0[i];

        state[11] = state[11]^sbox_table[state[15]];
        state[5] = state[5]^sbox_table[state[8]];

        for ( j = 0; j < STATEBYTES-1; j++ )
            state[j] = state[j+1];
        state[STATEBYTES-1] = fb_end;
        //wage_print_state(state);
    }
}

/*
 *convert_8bytes_to_7bitwords: convert 8 bytes (64 bits) into
 an array of length 10 of 7-bit words
 *inp: input of length 8
 *out: output of length 10 where output[9] contains 1 bit state information
*/
void convert_8bytes_to_7bitwords (unsigned char *out, const unsigned char *inp, u64 start_indx )
{
    unsigned char i, tmp, in[8];

    for ( i = 0; i < 8; i++ )
    {
        in[i] = 0x00;
        in[i] = in[i]^((inp[i+start_indx]&(0x01))<<7);
        in[i] = in[i]^((inp[i+start_indx]&(0x02))<<5);
        in[i] = in[i]^((inp[i+start_indx]&(0x04))<<3);
        in[i] = in[i]^((inp[i+start_indx]&(0x08))<<1);
        in[i] = in[i]^((inp[i+start_indx]&(0x10))>>1);
        in[i] = in[i]^((inp[i+start_indx]&(0x20))>>3);
        in[i] = in[i]^((inp[i+start_indx]&(0x40))>>5);
        in[i] = in[i]^((inp[i+start_indx]&(0x80))>>7);
    }
        out[0] = in[0]&mask;
        out[1] = (in[0]>>7)^(in[1]<<1);
        out[1] = out[1]&mask;
        out[2] = ((in[1]>>6))^(in[2]<<2);
        out[2] = out[2]&mask;
        out[3] = ((in[2]>>5))^(in[3]<<3);
        out[3] = out[3]&mask;
        out[4] = ((in[3]>>4))^(in[4]<<4);
        out[4] = out[4]&mask;
        out[5] = ((in[4]>>3))^(in[5]<<5);
        out[5] = out[5]&mask;
        out[6] = ((in[5]>>2))^(in[6]<<6);
        out[6] = out[6]&mask;
        out[7] = (in[6]>>1)^(in[7]<<7);
    out[7] = out[7]&mask;
    out[8] = in[7]&mask;
    out[9] = (in[7]>>7)&(0x1);
    for ( i = 0; i < 10; i++ )
    {
        tmp = 0x00;
        tmp^=((out[i]&(0x01))<<6);
        tmp^=((out[i]&(0x02))<<4);
        tmp^=((out[i]&(0x04))<<2);
        tmp^=(out[i]&(0x08));
        tmp^=((out[i]&(0x10))>>2);
        tmp^=((out[i]&(0x20))>>4);
        tmp^=((out[i]&(0x40))>>6);
        out[i] = tmp;
    }
        return;
}

/*
 *convert_7bitwords_to_8bytes: inverse function of convert_8bytes_to_7bitwords
 *inp: input of length 10
 *out: output of length 8
*/
void convert_7bitwords_to_8bytes (unsigned char *out, const unsigned char *inp, u64 start_indx )
{
    unsigned char i, tmp, in[10];
    for ( i = 0; i < 10; i++ )
    {
        tmp = 0x00;
        tmp^=((inp[i+start_indx]&(0x01))<<6);
        tmp^=((inp[i+start_indx]&(0x02))<<4);
        tmp^=((inp[i+start_indx]&(0x04))<<2);
        tmp^=(inp[i+start_indx]&(0x08));
        tmp^=((inp[i+start_indx]&(0x10))>>2);
        tmp^=((inp[i+start_indx]&(0x20))>>4);
        tmp^=((inp[i+start_indx]&(0x40))>>6);
        in[i] = tmp;
    }
        out[0] = in[0]^(in[1]<<7);
        out[1] = (in[1]>>1)^(in[2]<<6);
        out[2] = ((in[2]>>2))^(in[3]<<5);
        out[3] = ((in[3]>>3))^(in[4]<<4);
        out[4] = ((in[4]>>4))^(in[5]<<3);
        out[5] = ((in[5]>>5))^(in[6]<<2);
        out[6] = ((in[6]>>6))^(in[7]<<1);
        out[7] = (in[8])^(in[9]<<7);

        for ( i = 0; i < 8; i++ )
    {
        tmp = 0x00;
        tmp^=((out[i]&(0x01))<<7);
        tmp^=((out[i]&(0x02))<<5);
        tmp^=((out[i]&(0x04))<<3);
        tmp^=((out[i]&(0x08))<<1);
        tmp^=((out[i]&(0x10))>>1);
        tmp^=((out[i]&(0x20))>>3);
        tmp^=((out[i]&(0x40))>>5);
        tmp^=((out[i]&(0x80))>>7);
        out[i] = tmp;
    }

        return;
}

/*
 *convert_bytekey_to_wordkey: convert an array of length 16 byte
 into an array of length 19 of 7-bit words
 *inp: input
 *out: output
*/
void convert_bytekey_to_wordkey (unsigned char *out, const unsigned char *inp )
{
        unsigned char i, tmp, in[16];
        for ( i = 0; i < 16; i++ )
        {
                in[i] = 0x00;
                in[i] = in[i]^((inp[i]&(0x01))<<7);
                in[i] = in[i]^((inp[i]&(0x02))<<5);
                in[i] = in[i]^((inp[i]&(0x04))<<3);
                in[i] = in[i]^((inp[i]&(0x08))<<1);
                in[i] = in[i]^((inp[i]&(0x10))>>1);
                in[i] = in[i]^((inp[i]&(0x20))>>3);
                in[i] = in[i]^((inp[i]&(0x40))>>5);
                in[i] = in[i]^((inp[i]&(0x80))>>7);
        }
        out[0] = in[0]&mask;
        out[1] = (in[0]>>7)^(in[1]<<1);
        out[1] = out[1]&mask;
        out[2] = ((in[1]>>6))^(in[2]<<2);
        out[2] = out[2]&mask;
        out[3] = ((in[2]>>5))^(in[3]<<3);
        out[3] = out[3]&mask;
        out[4] = ((in[3]>>4))^(in[4]<<4);
        out[4] = out[4]&mask;
        out[5] = ((in[4]>>3))^(in[5]<<5);
        out[5] = out[5]&mask;
        out[6] = ((in[5]>>2))^(in[6]<<6);
        out[6] = out[6]&mask;
        out[7] = (in[6]>>1)^(in[7]<<7);
        out[7] = out[7]&mask;

        out[8] = in[7]&mask;

        out[18] = (in[7]>>7)&(0x1);
        out[18] = out[18]&mask;

        out[9] = in[8]&mask;
        out[10] = ((in[8]>>7))^(in[9]<<1);
        out[10] = out[10]&mask;
        out[11] = ((in[9]>>6))^(in[10]<<2);
        out[11] = out[11]&mask;
        out[12] = ((in[10]>>5))^(in[11]<<3);
        out[12] = out[12]&mask;
        out[13] = ((in[11]>>4))^(in[12]<<4);
        out[13] = out[13]&mask;
        out[14] = ((in[12]>>3))^(in[13]<<5);
        out[14] = out[14]&mask;
        out[15] = (in[13]>>2)^(in[14]<<6);
        out[15] = out[15]&mask;
        out[16] = (in[14]>>1)^(in[15]<<7);
        out[16] = out[16]&mask;

        out[17] = (in[15])&mask;

        out[18] = out[18]^((in[15]>>6)&(0x2));

        for ( i = 0; i < 19; i++ )
        {
                tmp = 0x00;
                tmp^=((out[i]&(0x01))<<6);
                tmp^=((out[i]&(0x02))<<4);
                tmp^=((out[i]&(0x04))<<2);
                tmp^=(out[i]&(0x08));
                tmp^=((out[i]&(0x10))>>2);
                tmp^=((out[i]&(0x20))>>4);
                tmp^=((out[i]&(0x40))>>6);
                out[i] = tmp;
        }
        return;
}

/*
 *convert_tag_word_to_byte: convert an array of length 19 of 7-bit words
 into an array of length 16 bytes.
 *inp: input of 7-bit words
 *out: output of bytes
 */
void convert_tag_word_to_byte (unsigned char *out, const unsigned char *inp )
{
        unsigned char i, tmp, in[19];
        for ( i = 0; i < 19; i++ )
        {
                tmp = 0x00;
                tmp^=((inp[i]&(0x01))<<6);
                tmp^=((inp[i]&(0x02))<<4);
                tmp^=((inp[i]&(0x04))<<2);
                tmp^=(inp[i]&(0x08));
                tmp^=((inp[i]&(0x10))>>2);
                tmp^=((inp[i]&(0x20))>>4);
                tmp^=((inp[i]&(0x40))>>6);
                in[i] = tmp;
        }
        out[0] = in[0]^(in[1]<<7);
        out[1] = (in[1]>>1)^(in[2]<<6);
        out[2] = ((in[2]>>2))^(in[3]<<5);
        out[3] = ((in[3]>>3))^(in[4]<<4);
        out[4] = ((in[4]>>4))^(in[5]<<3);
        out[5] = ((in[5]>>5))^(in[6]<<2);
        out[6] = ((in[6]>>6))^(in[7]<<1);
        out[7] = (in[8])^(in[9]<<7);

        out[8] = (in[9]>>1)^(in[10]<<6);
        out[9] = (in[10]>>2)^(in[11]<<5);
        out[10] = (in[11]>>3)^(in[12]<<4);
        out[11] = (in[12]>>4)^(in[13]<<3);
        out[12] = (in[13]>>5)^(in[14]<<2);
        out[13] = (in[14]>>6)^(in[15]<<1);
        out[14] = (in[16])^(in[17]<<7);
        out[15] = (in[17]>>1)^(in[18]<<6);
        for ( i = 0; i < 16; i++ )
        {
                tmp = 0x00;
                tmp^=((out[i]&(0x01))<<7);
                tmp^=((out[i]&(0x02))<<5);
                tmp^=((out[i]&(0x04))<<3);
                tmp^=((out[i]&(0x08))<<1);
                tmp^=((out[i]&(0x10))>>1);
                tmp^=((out[i]&(0x20))>>3);
                tmp^=((out[i]&(0x40))>>5);
                tmp^=((out[i]&(0x80))>>7);
                out[i] = tmp;
        }
        return;
}

/*
 *wage_print_state: print a state
*/
void wage_print_state( const unsigned char *state )
{
    unsigned char i;
    for ( i = 0; i < STATEBYTES; i++ )
        printf("%.2X", state[i]);
    printf("\n");
}

/*
 *wage_permutation_ALLZERO: compute output on input all-one (0^259)
 *state: output
 */
void wage_permutation_ALLZERO ( unsigned char *state )
{
    unsigned char i;

    for ( i = 0; i < STATEBYTES; i++ )
        state[i] = 0x00;
    wage_print_state(state);
    wage_permutation(state);
    wage_print_state(state);
return;
}

/*
 *wage_permutation_ALLONE: compute output on input all-one (1^259)
 *state: output
 */
void wage_permutation_ALLONE ( unsigned char *state )
{
    unsigned char i;

    for ( i = 0; i < STATEBYTES; i++ )
        state[i] = 0x0;
    //wage_print_state( state );
    wage_permutation(state);
    //wage_print_state( state );
return;
}
