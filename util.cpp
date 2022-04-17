// %W% %E% SBHRS

// Misc utility functions

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>

#include "rr.h"
#include "globals.h"
#include "util.h"

static
int mask2bitArray[] = {	//   1 2 3 4 5 6 7 8 
	   0x00,	//
	   0x01,	//   x
	   0x02,	//     x
	   0x04,	//       x
	   0x08,	//         x
	   0x10,	//           x
	   0x20,	//             x
	   0x40,	//               x
	   0x80 	//                 x
	   };

int mask2bit(int mask)
{

	for (int idx = 0; idx < 9; idx++) {
		if (mask2bitArray[idx] == mask) {
			// printf("%d\n", idx);
			return idx - 1;
		}
	}
	printf("mask2bit(0x%02x) => -1\n", mask);
	return -1;
}

static int my_bit2pin[2][25] = {
    // Old Style Chubb cards
    { 4,3,2,1,16,17,18,19,13,14,15,24,23,22,21,20,9,10,11,12,8,7,6,5 },
    // New Style Chubb cards
    { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 },
    };
static int my_pin2port[2][25];
static int my_pin2bit[2][25];
 

static void
init_pinbittables()
{
    static int pinbitinit = 0;
    char *sep = "";
    if (pinbitinit == 0) {
	for (int idx = 0; idx < 24; idx++) {
	    my_pin2port[0][my_bit2pin[0][idx]] = idx / 8;
	    my_pin2port[1][my_bit2pin[1][idx]] = idx / 8;
	    my_pin2bit[0][my_bit2pin[0][idx]] = idx % 8;
	    my_pin2bit[1][my_bit2pin[1][idx]] = idx % 8;
	}
	pinbitinit = 1;
#if 0
	printf("\npin2port:");
	for (int idx = 1; idx <= 24; idx++) {
	    printf("%s%d", sep, my_pin2port[0][idx]);
	    sep = ", ";
	}
	sep = "";
	printf("\npin2bit: ");
	for (int idx = 1; idx <= 24; idx++) {
	    printf("%s%d", sep, my_pin2bit[0][idx]);
	    sep = ", ";
	}
	exit(1);
#endif
    }
}

    /*
     1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
     3  2  1  0 23 22 21 20 15 16 17 18  9 10 11  4  5  6  7 15 14 13 12 11 
    */
int pin2port(int pin)
{
#if 1
    init_pinbittables();
    if ((pin < 1) || (pin > 24)) return -1;
    return  my_pin2port[0][pin];
#else
    for (int idx = 0; idx < 24; idx++) {
	if (my_bit2pin[0][idx] == pin) {
	    return idx / 8;
	}
    }
    return -1;
#endif
}

int pin2bit(int pin)
{
#if 1
    init_pinbittables();
    if ((pin < 1) || (pin > 24)) return -1;
    return  my_pin2bit[0][pin];
#else
	for (int idx = 0; idx < 24; idx++) {
		if (my_bit2pin[0][idx] == pin) {
			return idx % 8;
		}
	}
	return -1;
#endif
}

int
port2pin(int port, int bit)
{
	int idx = ((port - 1) % 3) * 8 + bit;
	if (idx < 0)  return -1;
	if (idx > 24) return -1;
	return my_bit2pin[0][idx];
}

char *
port2name(int port, int bit) {

	char b[25];
	char *ret;
	int pin = port2pin(port, bit);

	// sprintf(b, "%02d:%02d(%c:%1d)", (int)(port / 3), pin, 'A' + (port & 3), bit);
	sprintf(b, "%02d:%02d", (int)((port - 1) / 3), pin);
	ret = new char[strlen(b) + 1];
	strcpy(ret, b);
	return ret;
}


/*
**      Convert x into binary representation:
**
**                1         2         3
**      012345678901234567890123456789012345678
**      xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx
*/

char *binary(int maxbits, unsigned long x, char sep )
            /* 4,8,12,16,20,24,28, or 32 - how many bits printed? */
{
    static char binvalue[40];
    int bitpos, maxpos;

    /* initialize the buffer... */
    for (bitpos = 0; bitpos < 40; bitpos++) binvalue[bitpos] = ' ';

    bitpos = 0;

    if (sep != '\0') {
	/* save space for the '_'s every 4 bits */
	switch (maxbits) {  /* simplistic "log base x of maxbits" calculation... */
	    case  4: bitpos = 0;  break;
	    case  8: bitpos = 1;  break;
	    case 12: bitpos = 2;  break;
	    case 16: bitpos = 3;  break;
	    case 20: bitpos = 4;  break;
	    case 24: bitpos = 5;  break;
	    case 28: bitpos = 6;  break;
	    case 32: bitpos = 7;  break;
	}
    }

    bitpos += maxbits - 1;
    maxpos = bitpos;

    /* make sure the bufer is null terminated ... */
    binvalue[bitpos + 1] = '\0';

    /* ... and has at least one printable character */
    binvalue[bitpos    ] = '0';
    while (bitpos >= 0) {
	if (sep != '\0') {
	    if (bitpos == 34 ||
		bitpos == 29 ||
		bitpos == 24 ||
		bitpos == 19 ||
		bitpos == 14 ||
		bitpos ==  9 ||
		bitpos ==  4) {     /* every 4 bit positions */
		    binvalue[bitpos    ] = '_';
		    bitpos--;
	    }
	}
        if (x & 1)
            // binvalue[maxpos - bitpos    ] = '1';
            binvalue[bitpos] = '1';
        else
            // binvalue[maxpos - bitpos    ] = '0';
            binvalue[bitpos] = '0';
        bitpos--;
        x = x >> 1;     /* shift right by 1 bit position */
    }
    return binvalue;    /* so it can be printed... */
}

