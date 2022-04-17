// @(#)iotest.cc	1.2 99/08/27 SBHRS

// Walk thru datastructures

// #define NATIVE

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>

#include "rr.h"
#include "globals.h"
#include "cards.h"
#include "panel.h"

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

int my_bit2pin[2][25] = {
    // Old Style Chubb cards
    { 4,3,2,1,16,17,18,19,13,14,15,24,23,22,21,20,9,10,11,12,8,7,6,5 },
    // New Style Chubb cards
    { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 },
    };
int my_pin2port[2][25];
int my_pin2bit[2][25];
 

void
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

int
unassigned()
{
	printf("_________");
	return 1;
}


int
checkPanel(int in, int cardnumber, int port, int bit)
{
	int offset = (cardnumber - 1) * 3 + port + 1;

	if (in) {
	    for (int idx = 1; idx < 6 + 1; idx++) {
		if (panel[idx][PANEL_MAINLINE]  == offset) {
			if ((panel[idx][PANEL_MAINSHIFT] <= bit) &&
			    (panel[idx][PANEL_MAINSHIFT] + 2 > bit)) {
			    printf("Panel    %2d Mainline  Thumbwheel Port (bit%d)",
				idx, bit - panel[idx][PANEL_MAINSHIFT]);
			    return 1;
			}
		}
		if (panel[idx][PANEL_1PORT]  == offset) {
			if ((panel[idx][PANEL_1SHIFT] <= bit) &&
			    (panel[idx][PANEL_1SHIFT] + 4 > bit)) {
			    printf("Panel    %2d  1s Digit Thumbwheel Port (bit%d)",
				idx, bit - panel[idx][PANEL_1SHIFT]);
			    return 1;
			}
		}
		if (panel[idx][PANEL_10PORT]  == offset) {
			if ((panel[idx][PANEL_10SHIFT] <= bit) &&
			    (panel[idx][PANEL_10SHIFT] + 2 > bit)) {
			    printf("Panel    %2d 10s Digit Thumbwheel Port (bit%d)",
				idx, bit - panel[idx][PANEL_10SHIFT]);
			    return 1;
			}
		}
		if (panel[idx][PANEL_CABPORT]  == offset) {
			if ((panel[idx][PANEL_CABSHIFT] <= bit) &&
			    (panel[idx][PANEL_CABSHIFT] + 3 > bit)) {
			    printf("Panel    %2d CabNumber Thumbwheel Port (bit%d)",
				idx, bit - panel[idx][PANEL_CABSHIFT]);
			    return 1;
			}
		}
		if (PANEL_ASSIGN == offset) {
			if (mask2bit(panel[idx][PANEL_BUTTON]) == bit) {
			    printf("Panel    %2d Assign button", idx);
			    return 1;
			}
		}
		if (PANEL_DROP == offset) {
			if (mask2bit(panel[idx][PANEL_BUTTON]) == bit) {
			    printf("Panel    %2d Drop button", idx);
			    return 1;
			}
		}
	    }
	} else {
	    for (int idx = 1; idx < 6 + 1; idx++) {
		if (panel[idx][PANEL_BEEPER]  == offset) {
			if (mask2bit(panel[idx][PANEL_BEEPER_ON]) == bit) {
			    printf("Panel    %2d Buzzer", idx);
			    return 1;
			}
		}
	    }
	}
	return 0;
}

char *color[] = { "Red", "Yellow", "Green" };

int
checkSignal(int in, int cardnumber, int port, int bit)
{
	int offset = (cardnumber - 1) * 3 + port + 1;

	if (in) return 0; // Signals are output only

	for (int idx = 1; idx < NumberBlocks + 1; idx++) {
		if ( (blockbuf[idx][WSIGBASE] == 0) &&
		     (blockbuf[idx][ESIGBASE] == 0)) {
			continue;
		}
		if ( (blockbuf[idx][WSIGBASE]+0 == offset)
		   ||(blockbuf[idx][WSIGBASE]+1 == offset)
		   ||(blockbuf[idx][WSIGBASE]+2 == offset)) {
			if (blockbuf[idx][WSIGOFF] == bit) {
			    printf("W-Signal %2d %s",
			        blockbuf[idx][PHYSICAL],
				color[offset - blockbuf[idx][WSIGBASE]]);
			    return 1;
			}
		}
		if ( (blockbuf[idx][ESIGBASE]+0 == offset)
		   ||(blockbuf[idx][ESIGBASE]+1 == offset)
		   ||(blockbuf[idx][ESIGBASE]+2 == offset)) {
			if (blockbuf[idx][ESIGOFF] == bit) {
			    printf("E-Signal %2d %s",
			        blockbuf[idx][PHYSICAL],
				color[offset - blockbuf[idx][ESIGBASE]]);
			    return 1;
			}
		}
	}
	return 0;
}

int
checkDetector(int in, int cardnumber, int port, int bit)
{
	int offset = (cardnumber - 1) * 3 + port + 1;
	int idx;

	if (!in) return 0; // Detectors are input only

	for (idx = 1; idx < NumberBlocks + 1; idx++) {
		if (blockbuf[idx][DETBUF] == offset &&
		    blockbuf[idx][DETS]   == bit) {
			printf("Detector %2d (block %d)", idx,
			    blockbuf[idx][PHYSICAL]);
			return 1;
		}
	}

	for (idx = 1; idx < NumberSubs + 1; idx++) {
		if (subblockbuf[idx][0] == offset &&
		    subblockbuf[idx][1] == bit) {
			printf("SubBlock %2d (turnout %d, block %d/%d)", idx,
			    subblockbuf[idx][2], subblockbuf[idx][3], subblockbuf[idx][4]);
			return 1;
		}
	}
	return 0;
}

int
checkCab(int in, int cardnumber, int port, int bit)
{
	int offset = (cardnumber - 1) * 3 + port + 1;

	if (in) return 0; // Cabs are output only

	for (int idx = 1; idx < NumberBlocks + 1; idx++) {
		if (blockbuf[idx][OBUF] == offset) {
			if ((blockbuf[idx][OBUFS] <= bit) &&
			    (blockbuf[idx][OBUFS] + 0x03 > bit)) {
				printf("Block   %3d CabRelay card (bit%d)",
				    blockbuf[idx][PHYSICAL], bit - blockbuf[idx][OBUFS]);
				return 1;
			}
		}
	}
	return 0;
}

int
checkThrottle(int in, int cardnumber, int port, int bit)
{
	int offset = (cardnumber - 1) * 3 + port + 1;
	int idx;

	if (in) {
		for (idx = 1; idx < 8; idx++) {
			if (throttle[idx][0] == offset) {
				if (throttle[idx][1] == bit) {
				    printf("Throttle %2d Direction", idx);
				    return -1;
				}
			}
			if (throttle[idx][2] == offset) {
				if (throttle[idx][3] == bit) {
				    printf("Throttle %2d Button", idx);
				    return -1;
				}
			}
		}
	} else {
		for (idx = 1; idx < 8; idx++) {
			if (throttle[idx][4] == offset) {
				if (throttle[idx][5] == bit) {
				    printf("Throttle %2d LED", idx);
				    return -1;
				}
			}
		}
	}
	return 0;
}

int
checkTurnout(int in, int cardnumber, int port, int bit)
{
	int retval = 0;
	int offset = (cardnumber - 1) * 3 + port + 1;

	for (int idx = 1; idx < NumberTurnouts + 1; idx++) {
		if (in) {
			if (turnout[idx][POSITION_PORT] == offset &&
			    turnout[idx][POSITION_BITS] == bit) {
				char *mot = port2name(turnout[idx][CONTROL_PORT],
				                      turnout[idx][CONTROL_BITS]);
				char *man = port2name(turnout[idx][MANUAL_PORT],
						      turnout[idx][MANUAL_BITS]);
				printf("Turnout %3d Position [mot: o%s, man: i%s]",
					 idx, mot, man);
				  retval = 1;
				  delete[] mot;
				  delete[] man;
				break;
			}
			if (turnout[idx][MANUAL_PORT] == offset &&
			    turnout[idx][MANUAL_BITS] == bit) {
				char *pos = port2name(turnout[idx][POSITION_PORT],
						      turnout[idx][POSITION_BITS]);
				char *mot = port2name(turnout[idx][CONTROL_PORT],
						      turnout[idx][CONTROL_BITS]);
				printf("Turnout %3d Manual   [mot: o%s, pos: i%s]",
					idx, mot, pos);
				  retval = 1;
				  delete[] mot;
				  delete[] pos;
				break;
			}
		} else {
			if (turnout[idx][CONTROL_PORT] == offset &&
			    turnout[idx][CONTROL_BITS] == bit) {
				char *pos = port2name(turnout[idx][POSITION_PORT],
						      turnout[idx][POSITION_BITS]);
				char *man = port2name(turnout[idx][MANUAL_PORT],
 						      turnout[idx][MANUAL_BITS]);
				printf("Turnout %3d Motor    [pos: i%s, man: i%s]",
				  	idx, pos, man);
				  retval = 1;
				  delete[] pos;
				  delete[] man;
				break;
			}
		}
		if ((turnout[idx][POSITION_PORT] == turnout[idx][MANUAL_PORT]) &&
		    (turnout[idx][POSITION_BITS] == turnout[idx][MANUAL_BITS])) {
			printf(" INVALID");
			continue;
		}
	}
	return retval;
}

static int checkCard(
		int in,
		int cardnumber,
		int port,
		int bit)
{
	int matches =
	checkTurnout (in, cardnumber, port, bit) +
	checkDetector(in, cardnumber, port, bit) +
	checkPanel   (in, cardnumber, port, bit) +
	checkSignal  (in, cardnumber, port, bit) +
	checkThrottle(in, cardnumber, port, bit) +
	checkCab     (in, cardnumber, port, bit);

	if (matches == 0) {
		unassigned   ();
	}
	printf("\n");

	return matches;
}

void
printCard(int in, int card_address, int cardnumber, int a, int b, int c)
{
	char *ctype;
	char lead[255];
	char init[10];

	printf("\f");
	if (in) {
		printf("Card %2d  i%d  INPUT (InBuf[%2d,%2d,%2d])\n",
				card_address, cardnumber,
				(cardnumber * 3) + 1,
				(cardnumber * 3) + 2,
				(cardnumber * 3) + 3
				);
		printf("-----------------------------------------\n");
		ctype = "i";
	} else {
		printf("Card %2d  o%d  OUTPUT (OutBuf[%2d,%2d,%2d])\n",
				card_address, cardnumber,
				(cardnumber * 3) + 1,
				(cardnumber * 3) + 2,
				(cardnumber * 3) + 3
				);
		printf("-----------------------------------------\n");
		ctype = "o";
	}

	//--
	for (int pin = 24; pin >= 1; pin--) {
	 	int port = pin2port(pin);
	 	int bit  = pin2bit(pin);
	
	/*
	for (int port = 0; port <= 2; port++) 
		for (int bit = 0; bit <= 7; bit++) {  // }
			int pin = port2pin(port, bit);
			*/

			if (!in) {
			    switch (port) {
			    case 0: sprintf(init, "(%d)", (a >> bit) & 0x01); break;
			    case 1: sprintf(init, "(%d)", (b >> bit) & 0x01); break;
			    case 2: sprintf(init, "(%d)", (c >> bit) & 0x01); break;
			    }
			} else {
			    sprintf(init, "   ");
			}
			
			// Show card and pin numbers
			printf("Card %02d:%02d ", card_address, pin);

			// Show in/out buffer address/bitpos
			printf("%s[%03d]:%d (%c:%d)",
				(strcmp(ctype, "i") == 0) ? "InBuf" : "OutBuf",
				(cardnumber * 3) + (port % 3) + 1,
				bit,
				'A' + (port % 3),
				bit);

			printf(" %s", init);
			checkCard(in, cardnumber + 1, port, bit);
	}
	printf("\n");
	// printf("\f");
}

void print_turnouts(void) {
	for (int idx = 1; idx < NumberTurnouts + 1; idx++) {

		char *mot = port2name(turnout[idx][CONTROL_PORT],
				      turnout[idx][CONTROL_BITS]);
		char *man = port2name(turnout[idx][MANUAL_PORT],
				      turnout[idx][MANUAL_BITS]);
		char *pos = port2name(turnout[idx][POSITION_PORT],
				      turnout[idx][POSITION_BITS]);
		printf("%2d: Motor: o%s Position: i%s Manual: i%s\n",
				idx, mot, pos, man);
	}
}
int
main(int argc, char *argv[])
{
	int inputcard = 0;
	int outputcard = 0;
	int undefinedcard = 0;
	BYTE     *OutBuf;   // Output Buffer

	// printf("C/MRI I/O Worksheet for I/O assignments for SBHRS\n");

	Cards *IOcards = new Cards();
        OutBuf  = new BYTE[IOcards->OutputBufferSize()];
        InitOutBuf(OutBuf);

	IOcards->print();

	printf("\f");

	for (int x = 0; x < 16; x++) {
		for (int y = 0; y <= 3; y++) {
			int c = (IOcards->CardType[x] >> (y*2)) & 0x03;
			if (c == 1) {
				printCard(1, (x * 4 + y), inputcard,0,0,0);
				inputcard++;
			} else if (c == 2) {
				int idx = (outputcard - 1) * 3 + 1;
				printCard(0, (x * 4 + y), outputcard,
					OutBuf[idx++],
					OutBuf[idx++],
					OutBuf[idx]);
				outputcard++;
			} else {
				undefinedcard++;
			}
		}
	}
	printf("\f");
	print_turnouts();
}

