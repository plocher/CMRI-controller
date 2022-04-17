// @(#)test.cc	1.2 99/08/18 SBHRS

// #define NATIVE
#define TEST

#define DEBUG_logic /* */
/* #define DEBUG_Inputbits /* */
/* #define DEBUG_Outputbits /* */

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
// #include <unistd.h>

#include <iostream.h>
#include <string.h>
#include <math.h>

#include "rr.h"
#include "cards.h"
#include "port.h"  // Port comunications
#include "usic.h"  // USIC comunications

#define WB1a	0	//	double head EB signal at turnout points
#define WB1b	1
#define SIDING	2	//	siding WB frog
#define EB3	3	//	mainline WB frog
#define EB4	4	//	mainline EB
#define WB4	5	//	mainline WB


#define ASPECT_OFF	0
#define ASPECT_STOP	1
#define ASPECT_APROACH	2
#define ASPECT_CLEAR	3
#define ASPECT_ON	4


#define getBit(buffer, address, bit) \
	(( (buffer) [ (address) ] >> (bit + 1) ) & 1)

#define setBit(buffer, address, bit) \
	(( (buffer) [ (address) ] ) |= (0x01 << (bit - 1)) )

#define clearBit(buffer, address, bit) \
	(( (buffer) [ (address) ] ) &= ~(0x01 << (bit - 1)) )


struct	turnout_t {
	int	route;
#define	NORMAL		0
#define DIVERGING	1
	int	lock;
#define	CTC		0
#define LOCAL	1
};

struct rr_t {
	Cards	 *IOcards;
	USIC	 *usic;

	BYTE     *OutBuf;   // Output Buffer
	BYTE     *InBuf;    // Input Buffer
	BYTE     *SavBuf;  // Copy of Output Buffer


	int		   block[6];
	struct	turnout_t  turnout[1];
	int		   sig[7];
} railroad;


void set_sig_bits(struct rr_t *railroad,
	     int aspect,
	     int rp, int rb,
	     int yp, int yb,
	     int gp, int gb)
{
	clearBit(railroad->OutBuf, rp, rb);
	clearBit(railroad->OutBuf, yp, yb);
	clearBit(railroad->OutBuf, gp, gb);

	if ((aspect == ASPECT_STOP)    ||
	    (aspect == ASPECT_ON))	 { setBit(railroad->OutBuf, rp, rb); }
	if ((aspect == ASPECT_APROACH) ||
	    (aspect == ASPECT_ON))	 { setBit(railroad->OutBuf, yp, yb); }
	if ((aspect == ASPECT_CLEAR)   ||
	    (aspect == ASPECT_ON))	 { setBit(railroad->OutBuf, gp, gb); }
}

void Input(struct rr_t *railroad)
{
	// railroad->usic.Read(railroad->InBuf);
	railroad->InBuf[1] = 0x83;
	railroad->InBuf[2] = 0x00;
	railroad->InBuf[3] = 0x00;
	
#ifdef DEBUG_Inputbits
	printf("Input: ");
	printf(" %s", binary(8,(unsigned long)railroad->InBuf[1]));
    	printf(" %s", binary(8,(unsigned long)railroad->InBuf[2]));
    	printf(" %s", binary(8,(unsigned long)railroad->InBuf[3]));
	printf("\n");
#endif
	railroad->block[0] = getBit(railroad->InBuf, 1, 1);
	railroad->block[1] = getBit(railroad->InBuf, 1, 2);
	railroad->block[2] = getBit(railroad->InBuf, 1, 3);
	railroad->block[3] = getBit(railroad->InBuf, 1, 4);
	railroad->block[4] = getBit(railroad->InBuf, 1, 5);
	railroad->block[5] = getBit(railroad->InBuf, 1, 6);

	railroad->turnout[0].lock  = getBit(railroad->InBuf, 1, 7);
	railroad->turnout[0].route = getBit(railroad->InBuf, 1, 8);
}

void Process(struct rr_t *railroad)
{
	static int aspect = ASPECT_OFF;
	static int x = WB1a;

	railroad->sig[WB1a]   = ASPECT_OFF;
	railroad->sig[WB1b]   = ASPECT_OFF;
	railroad->sig[SIDING] = ASPECT_OFF;
	railroad->sig[EB3]    = ASPECT_OFF;
	railroad->sig[EB4]    = ASPECT_OFF;
	railroad->sig[WB4]    = ASPECT_OFF;

	railroad->sig[x] = aspect;

	if (aspect == ASPECT_ON) {
		aspect = ASPECT_OFF;
		x++;
	}
	aspect++;
	if (x > WB4) exit(1);

	/* 
	if (railroad->turnout[0].route == NORMAL) {
		railroad->sig[WB1a]   = ASPECT_CLEAR;
		railroad->sig[WB1b]   = ASPECT_STOP;
		railroad->sig[SIDING] = ASPECT_STOP;
		railroad->sig[WB4]    = ASPECT_CLEAR;
		railroad->sig[EB3]    = ASPECT_CLEAR;
		railroad->sig[EB4]    = ASPECT_CLEAR;
	} else {	// Diverging
		railroad->sig[WB1a]   = ASPECT_STOP;
		railroad->sig[WB1b]   = ASPECT_CLEAR;
		railroad->sig[SIDING] = ASPECT_CLEAR;
		railroad->sig[WB4]    = ASPECT_STOP; // mainline WB
		railroad->sig[EB3]    = ASPECT_STOP;
		railroad->sig[EB4]    = ASPECT_APROACH;
	}
	*/
}

void Output(struct rr_t *railroad)
{
#ifdef DEBUG_logic
	for (int x = 0; x < 6; x++) {
		if (railroad->block[x]) printf("x"); else printf(".");
	}
	printf(" ");

	for (int x = 0; x < 6; x++) {
		if      (railroad->sig[x]==ASPECT_STOP)    printf("r");
		else if (railroad->sig[x]==ASPECT_APROACH) printf("y");
		else if (railroad->sig[x]==ASPECT_CLEAR)   printf("g");
		else if (railroad->sig[x]==ASPECT_OFF)     printf("-");
		else if (railroad->sig[x]==ASPECT_ON)      printf("*");
		else					   printf("?");
	}
	printf(" ");
	if (railroad->turnout[0].route == NORMAL) printf("N"); else printf("D");
	if (railroad->turnout[0].lock  == CTC)    printf("."); else printf("M");
	printf("\n");
#endif

	set_sig_bits(railroad, railroad->sig[WB1a],   1,1, 1,2, 1,3);
	set_sig_bits(railroad, railroad->sig[WB1b],   1,4, 1,5, 1,6);
	set_sig_bits(railroad, railroad->sig[SIDING], 1,7, 1,8, 2,1);
	set_sig_bits(railroad, railroad->sig[WB4],    2,2, 2,3, 2,4);
	set_sig_bits(railroad, railroad->sig[EB3],    2,5, 2,6, 2,7);
	set_sig_bits(railroad, railroad->sig[EB4],    2,8, 3,1, 3,2);

#ifdef DEBUG_Outputbits
	printf("Output:");
	printf(" %s", binary(8,(unsigned long)railroad->OutBuf[1]));
    	printf(" %s", binary(8,(unsigned long)railroad->OutBuf[2]));
    	printf(" %s", binary(8,(unsigned long)railroad->OutBuf[3]));
	printf("\n");
#endif
	// usic.Write(OutBuf);
}



int
main(int argc, char *argv[])
{
        int y, x = 0;
	int done;

	setbuf (stdout, NULL);

	printf("Chubb Test Program\n");

   	printf("Initializing Card Arrays\n");

		railroad.IOcards = new Cards(
				"O"	//  0: LED Test card
				"I"	//  1: Input
				);
		railroad.IOcards->print();

   	printf("Allocating Buffers\n");

		railroad.OutBuf = new BYTE[railroad.IOcards->OutputBufferSize()];
		railroad.SavBuf = new BYTE[railroad.IOcards->OutputBufferSize()];
		railroad.InBuf  = new BYTE[railroad.IOcards->InputBufferSize()];

		for (x=0; x < railroad.IOcards->NumberOutputPorts(); x++) {
			railroad.OutBuf[x] = 0;
			railroad.SavBuf[x] = 0;
		}
		for (x=0; x < railroad.IOcards->NumberInputPorts(); x++) {
			railroad.InBuf[x]  = 0;
		}

   	printf("Initializing USIC\n");

		// railroad.usic = new USIC(railroad->IOcards, 0, Port::COM1, 9600);
	
	railroad.sig[0] = ASPECT_STOP;
	railroad.sig[1] = ASPECT_STOP;
	railroad.sig[2] = ASPECT_STOP;
	railroad.sig[3] = ASPECT_STOP;
	railroad.sig[4] = ASPECT_STOP;
	railroad.sig[5] = ASPECT_STOP;
	railroad.sig[6] = ASPECT_STOP;

	while (1) {

		if (kbhit()) { getch(); break; }
		Input(&railroad);
		Process(&railroad);
		Output(&railroad);
	}

}

/*

void fli()
{
   	printf("Write test (lights should flash):  ");
		y = 0;
		bit=0;
		for (;;) {
			for (x=1; x <= IOcards->NumberOutputPorts(); x++) {
				OutBuf[x] = y;
			}
			y = (y + 1) % 0xff;
			usic.Write(OutBuf);
			// if (y == 3) exit(1);
			if (kbhit()) { getch(); break; }
		}

   	printf("\nRead test:\n");

		usic.Read(InBuf);
		printf("Input: 0x%02x 0x%02x 0x%02x\n",
				InBuf[1], InBuf[2], InBuf[3]);

	printf("Read/write test\n");
	printf("    (connect pin 1 of input card to one of the outputs)\n");
	printf("    (connect pin 2 of input card to +5v to exit test)\n");
		y = 0;
		for(done=0; !done;) {
			if (loop > 7) {
				bi++;
				loop= 0;
			}
			if (bi > 3) {
				bi = 0;
				loop = 0;
				bits[0] = bits[1] = bits[2] = 0;
			}
			usic.Read(InBuf);
			// printf("Input: 0x%02x 0x%02x 0x%02x\n", InBuf[1], InBuf[2], InBuf[3]);

			if ((InBuf[1] & 0x01) != 0) {
				bi=0;
				loop=0;
				bits[0] = bits[1] = bits[2] = 0;
			} else {
				bits[bi] += pow(2, loop++);
			}

			if ((InBuf[1] & 0x02) != 0) {
				// done=1;
			}

			OutBuf[1] = bits[0];
			OutBuf[2] = bits[1];
			OutBuf[3] = bits[2];

			usic.Write(OutBuf);
			if (kbhit()) { getch(); break; }
		}
		if (!done) {
			printf("Aborted\n");
		} else {
			printf("Done\n");
		}
	}
*/


