// %W% %E% SBHRS

#include <ctype.h>
#include "rr.h"
#include "globals.h"
#include "usic.h"
#include "panel.h"
#include "util.h"
#include "cards.h"

extern void P(int line, char *str);

static void draw_one_card(USIC *usic, int card);
static void draw_all_cards(USIC *usic);
static void initcard(USIC *usic);
static char *checkCard(int in, int cardnumber, int port, int bit);

static int firsttime=1;
static int card2idx[64];
static char **cardassignments[64];


static void
screen_background(void)
{
    int i;

    textbackground(LIGHTGRAY);
    clrscr();
//	       1        2         3        4         5        6         7
//    1234567890123456780123456789012345678012345678901234567801234567890123456789012
P( 1,"+------------------------------------------------------------------------------+");
P( 2,"| TITLE goes here (See dspmnu() for details)                                   |");
P( 3,"| Commands menu goes here                                                      |");
P( 4,"|------------------------------------------------------------------------------|");
P( 5,"|                                                                              |");
P( 6,"|                                                                              |");
P( 7,"|                                                                              |");
P( 8,"|                                                                              |");
P( 9,"|                                                                              |");
P(10,"|                                                                              |");
P(11,"|                                                                              |");
P(12,"|                                                                              |");
P(13,"|                                                                              |");
P(14,"|                                                                              |");
P(15,"|                                                                              |");
P(16,"|                                                                              |");
P(17,"|                                                                              |");
P(18,"|                                                                              |");
P(19,"|                                                                              |");
P(20,"|                                                                              |");
P(21,"|                                                                              |");
P(22,"|                                                                              |");
P(23,"|                                                                              |");
P(24,"|                                                                              |");
P(25,"+------------------------------------------------------------------------------");

    // corners
    gotoxy( 1, 1); putch((char)218); // |--
    gotoxy(80, 1); putch((char)191); // --|
    gotoxy( 1,25); putch((char)192); // |__
 // gotoxy(80,25); putch((char)188); // __|  188	scrolls screen!
    gotoxy( 1, 4); putch((char)195); // |-
    gotoxy(80, 4); putch((char)180); // -|

}

void
draw_cardio_screen(int refresh, USIC *usic)
{
	static int lastcard = -1;
	if ((lastcard != options.cardio_card) ||
            (refresh) ) {
		lastcard = options.cardio_card;
		screen_background(); // blank screen ...
		dspmnu();
	}
	if (lastcard == -1) {
		draw_all_cards(usic);
	} else {
		draw_one_card(usic, options.cardio_card);
	}
}

static void
draw_one_card(USIC *usic, int card)
{
	int pin;
	int x,y;
	int ct;
#define topline 7
	int line = topline, col = 3;

	if (firsttime) {
		initcard(usic);
	}

	x = (int)(card / 4);
	y = (int)(card % 4);
	ct = (usic->ioCards->CardType[x] >> (y*2)) & 0x03;

	gotoxy(2, 5);
        if (ct == 1) {
		textcolor(BLUE);
		cprintf("Input card: %d", card);
	} else {
		textcolor(BROWN);
		cprintf("Output card: %d", card);
	}
	textcolor(BLACK);
	for (pin = 24; pin > 0; pin--) {
		int port = pin2port(pin);
		int bit  = pin2bit(pin);
		int idx;

		gotoxy(col, line++);
		if (line >= (topline + 12)) { 
			line = topline;
			col += 39;
		}
		if (ct == 1) {
			idx = InBuf[card2idx[card] + port];
		} else {
			idx = OutBuf[card2idx[card] + port];
		}
		cprintf("Pin %02d: ", pin);
		if (((idx >> bit) & 0x01) == 1) {
			textcolor(RED);
			cprintf("ON ");
		} else {
			textcolor(BLUE);
			cprintf("OFF");
		}
		textcolor(BLACK);
		cprintf(" %s", cardassignments[card][pin]);
	}
}

static void
draw_all_cards(USIC *usic)
{

	int i;
	int Iline = 4;
	int line = Iline;
	int col = 2;
	int card = 0;

	int inputcard = 0;
	int outputcard = 0;

	char buf[40];

	if (firsttime) {
		initcard(usic);
	}

        textcolor(BLACK);
	for (int x = 0; x < 16; x++) {
	    for (int y = 0; y <= 3; y++) {
		int ct = (usic->ioCards->CardType[x] >> (y*2)) & 0x03;

		if (line >= 25) {
		    line = Iline;
		    col += 26;
		    if (col >= 80) continue;
		}
		gotoxy(col, line++);

		textcolor(((ct == 1) ? YELLOW : RED));
		cprintf("%02d", (x * 4 + y));
		textcolor(((ct == 1) ? BLUE : BROWN));

		buf[0] = '\0';
		for (int pin = 24; pin >= 1; pin--) {
		    int port = pin2port(pin);
		    int bit  = pin2bit(pin);
		    int idx;

		    if (ct == 1) {
			// idx = InBuf[inputcard * 3 + 1 + port];
			idx = InBuf[card2idx[card] + port];
		    } else {
			idx = OutBuf[card2idx[card] + port];
		    }
		    strcat(buf, (((idx >> bit) & 0x01) == 1) ? "1" : "0");
		}
		cprintf(buf);
		if (ct == 1) {
		    inputcard++;
		} else {
		    outputcard++;
		}
		card++;
	    }
	}

        textcolor(BLACK);
	gotoxy( 3,22);
}


static void
initcard(USIC *usic)
{
	int x,y, pin;
	int inputcard = 0;
	int outputcard = 0;
	int c = 0;

	firsttime = 0;

	for (x = 0; x < 16; x++) {
	    for (y = 0; y <= 3; y++) {
		int ct = (usic->ioCards->CardType[x] >> (y*2)) & 0x03;

		cardassignments[c] = (char * *)calloc(25, sizeof(char *));
		cardassignments[c][0]="pins are [1-24]";

		if (ct == 1) {
		    card2idx[c] = (inputcard * 3 + 1);
		} else {
		    card2idx[c] = (outputcard * 3 + 1);
		}
		for (pin = 1; pin <= 24; pin++) {
		    int port = pin2port(pin);
		    int bit  = pin2bit(pin);
		    cardassignments[c][pin]=checkCard((ct == 1), (ct == 1)? inputcard : outputcard, port, bit);
		}
		if (ct == 1) {
		    inputcard++;
		} else {
		    outputcard++;
		}
		c++;
	    }
	}
}


// Walk thru datastructures

char *
unassigned(
	    int in,
	    int cardnumber,
	    int port,
	    int bit)
{
	char buffer[80];
	sprintf(buffer, "_%d %2d_%2d_%2d_", in, cardnumber, port, bit);
	return strdup(buffer);
}


char *
checkPanel(int in, int cardnumber, int port, int bit)
{
	int offset = cardnumber * 3 + 1 + port;
	char buffer[80];

	if (in) {
	    for (int idx = 1; idx < 6 + 1; idx++) {
		if (panel[idx][PANEL_MAINLINE]  == offset) {
			if ((panel[idx][PANEL_MAINSHIFT] <= bit) &&
			    (panel[idx][PANEL_MAINSHIFT] + 2 > bit)) {
			    sprintf(buffer, "Panel %2d Mline wheel (b%d)",
				idx, bit - panel[idx][PANEL_MAINSHIFT]);
			    return strdup(buffer);
			}
		}
		if (panel[idx][PANEL_1PORT]  == offset) {
			if ((panel[idx][PANEL_1SHIFT] <= bit) &&
			    (panel[idx][PANEL_1SHIFT] + 4 > bit)) {
			    sprintf(buffer,"Panel %2d  1s wheel (b%d)",
				idx, bit - panel[idx][PANEL_1SHIFT]);
			    return strdup(buffer);
			}
		}
		if (panel[idx][PANEL_10PORT]  == offset) {
			if ((panel[idx][PANEL_10SHIFT] <= bit) &&
			    (panel[idx][PANEL_10SHIFT] + 2 > bit)) {
			    sprintf(buffer, "Panel %2d 10s wheel (b%d)",
				idx, bit - panel[idx][PANEL_10SHIFT]);
			    return strdup(buffer);
			}
		}
		if (panel[idx][PANEL_CABPORT]  == offset) {
			if ((panel[idx][PANEL_CABSHIFT] <= bit) &&
			    (panel[idx][PANEL_CABSHIFT] + 3 > bit)) {
			    sprintf(buffer, "Panel %2d CabNum wheel (b%d)",
				idx, bit - panel[idx][PANEL_CABSHIFT]);
			    return strdup(buffer);
			}
		}
		if (PANEL_ASSIGN == offset) {
			if (mask2bit(panel[idx][PANEL_BUTTON]) == bit) {
			    sprintf(buffer, "Panel %2d Assign button", idx);
			    return strdup(buffer);
			}
		}
		if (PANEL_DROP == offset) {
			if (mask2bit(panel[idx][PANEL_BUTTON]) == bit) {
			    sprintf(buffer, "Panel %2d Drop button", idx);
			    return strdup(buffer);
			}
		}
	    }
	} else {
	    for (int idx = 1; idx < 6 + 1; idx++) {
		if (panel[idx][PANEL_BEEPER]  == offset) {
			if (mask2bit(panel[idx][PANEL_BEEPER_ON]) == bit) {
			    sprintf(buffer, "Panel %2d Buzzer", idx);
			    return strdup(buffer);
			}
		}
	    }
	}
	return NULL;
}

char *
checkTurnout(int in, int cardnumber, int port, int bit)
{
	int offset = cardnumber * 3 + 1 + port;
	char buffer[80];
	buffer[0]='\0';

	for (int idx = 1; idx < NumberTurnouts + 1; idx++) {
		if (in) {
			if (turnout[idx][POSITION_PORT] == offset &&
			    turnout[idx][POSITION_BITS] == bit) {
				char *mot = port2name(turnout[idx][CONTROL_PORT],
				                      turnout[idx][CONTROL_BITS]);
				char *man = port2name(turnout[idx][MANUAL_PORT],
						      turnout[idx][MANUAL_BITS]);
				sprintf(buffer, "Tout %3d Pos M%s L%s",
					 idx, mot, man);
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
				sprintf(buffer, "Tout %3d Lck M%s P%s",
					idx, mot, pos);
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
				sprintf(buffer,"Tout %3d Mot P%s L%s",
				  	idx, pos, man);
				  delete[] pos;
				  delete[] man;
				break;
			}
		}
		if ((turnout[idx][POSITION_PORT] == turnout[idx][MANUAL_PORT]) &&
		    (turnout[idx][POSITION_BITS] == turnout[idx][MANUAL_BITS])) {
			buffer[8] =  '#';
			continue;
		}
	}
	if (strlen(buffer) == 0) return NULL;
	return strdup(buffer);
}

char *
checkCab(int in, int cardnumber, int port, int bit)
{
	int offset = cardnumber * 3 + 1 + port;
	char buffer[80];
	buffer[0]='\0';

	if (in) return NULL; // Cabs are output only

	for (int idx = 1; idx < NumberBlocks + 1; idx++) {
		if (blockbuf[idx][OBUF] == offset) {
			if ((blockbuf[idx][OBUFS] <= bit) &&
			    (blockbuf[idx][OBUFS] + 0x03 > bit)) {
				sprintf(buffer, "Blk %3d CabRelay (b%d)",
				    blockbuf[idx][PHYSICAL], bit - blockbuf[idx][OBUFS]);
				return strdup(buffer);
			}
		}
	}
	return NULL;
}


char *color[] = { "Red", "Yellow", "Green" };

char *
checkSignal(int in, int cardnumber, int port, int bit)
{
	int offset = cardnumber * 3 + 1 + port;
	char buffer[80];
	buffer[0]='\0';

	if (in) return NULL; // Signals are output only

	for (int idx = 1; idx < NumberBlocks + 1; idx++) {
		if ( (blockbuf[idx][WSIGBASE] == 0) &&
		     (blockbuf[idx][ESIGBASE] == 0)) {
			continue;
		}
		if ( (blockbuf[idx][WSIGBASE]+0 == offset)
		   ||(blockbuf[idx][WSIGBASE]+1 == offset)
		   ||(blockbuf[idx][WSIGBASE]+2 == offset)) {
			if (blockbuf[idx][WSIGOFF] == bit) {
			    sprintf(buffer, "W-Signal %2d %s",
			        blockbuf[idx][PHYSICAL],
				color[offset - blockbuf[idx][WSIGBASE]]);
			    return strdup(buffer);
			}
		}
		if ( (blockbuf[idx][ESIGBASE]+0 == offset)
		   ||(blockbuf[idx][ESIGBASE]+1 == offset)
		   ||(blockbuf[idx][ESIGBASE]+2 == offset)) {
			if (blockbuf[idx][ESIGOFF] == bit) {
			    sprintf(buffer, "E-Signal %2d %s",
			        blockbuf[idx][PHYSICAL],
				color[offset - blockbuf[idx][ESIGBASE]]);
			    return strdup(buffer);
			}
		}
	}
	return NULL;
}

char *
checkDetector(int in, int cardnumber, int port, int bit)
{
	int offset = cardnumber * 3 + 1 + port;
	int idx;
	char buffer[80];
	buffer[0]='\0';

	if (!in) return 0; // Detectors are input only

	for (idx = 1; idx < NumberBlocks + 1; idx++) {
		if (blockbuf[idx][DETBUF] == offset &&
		    blockbuf[idx][DETS]   == bit) {
			sprintf(buffer, "Det %2d blk %d", idx,
			    blockbuf[idx][PHYSICAL]);
			return strdup(buffer);
		}
	}

	for (idx = 1; idx < NumberSubs + 1; idx++) {
		if (subblockbuf[idx][0] == offset &&
		    subblockbuf[idx][1] == bit) {
			sprintf(buffer, "Sub %2d (tout %d, blk %d/%d)", idx,
			    subblockbuf[idx][2], subblockbuf[idx][3], subblockbuf[idx][4]);
			return strdup(buffer);
		}
	}
	return NULL;
}

char *
checkThrottle(int in, int cardnumber, int port, int bit)
{
	int offset = cardnumber * 3 + 1 + port;
	int idx;
	char buffer[80];
	buffer[0]='\0';

	if (in) {
		for (idx = 1; idx < 8; idx++) {
			if (throttle[idx][0] == offset) {
				if (throttle[idx][1] == bit) {
				    sprintf(buffer, "Thr %2d Dir", idx);
				    return strdup(buffer);
				}
			}
			if (throttle[idx][2] == offset) {
				if (throttle[idx][3] == bit) {
				    sprintf(buffer, "Thr %2d But", idx);
				    return strdup(buffer);
				}
			}
		}
	} else {
		for (idx = 1; idx < 8; idx++) {
			if (throttle[idx][4] == offset) {
				if (throttle[idx][5] == bit) {
				    sprintf(buffer, "Thr %2d LED", idx);
				    return strdup(buffer);
				}
			}
		}
	}
	return NULL;
}


static char * checkCard(
		int in,
		int cardnumber,
		int port,
		int bit)
{
	char *s;

	if ( (s=checkTurnout (in, cardnumber, port, bit)) ) return s;
	if ( (s=checkDetector(in, cardnumber, port, bit)) ) return s;
	if ( (s=checkPanel   (in, cardnumber, port, bit)) ) return s;
	if ( (s=checkSignal  (in, cardnumber, port, bit)) ) return s;
	if ( (s=checkThrottle(in, cardnumber, port, bit)) ) return s;
	if ( (s=checkCab     (in, cardnumber, port, bit)) ) return s;
	return unassigned    (in, cardnumber, port, bit);
}

#if 0

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

int
testwiremain(int argc, char *argv[])
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
#endif


