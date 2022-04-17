// %W% %E% SBHRS

// #define NATIVE
#define TEST

/* #define DEBUG_Inputbits /* */
/* #define DEBUG_Outputbits /* */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <iostream.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "rr.h"
#include "cards.h"
#include "globals.h"
#include "port.h"  // Port comunications
#include "usic.h"  // USIC comunications
#include "panel.h"
#include "util.h"

struct rr_t {
	Cards	 *IOcards;
	USIC	 *usic;

	BYTE     *OutBuf;   // Output Buffer
	BYTE     *InBuf;    // Input Buffer
	BYTE     *SavBuf;  // Copy of Output Buffer

	int cardnum;
	int portnum;
};

extern void draw_cardio_screen(int refresh, struct rr_t *railroad);
extern void check(struct rr_t *railroad);

static int menuxpos;
static const char far* menu;

static void cPrintString(char *s)
{
	char *sp = s;
	while (*sp) {
		switch (*sp) {
			case '=': cprintf("%c", (char)205); break;
			case '-': cprintf("%c", (char)196); break;
			case '<': cprintf("%c", (char)174); break;
			case '>': cprintf("%c", (char)175); break;
			case '|': cprintf("%c", (char)179); break;
			default: 
				  if ( isalpha(*sp) && !isdigit(*(sp+1)) ) {
					  textcolor(BROWN);
					  cprintf("%c", *sp);
					  textcolor(BLACK);
				  } else {
					  cprintf("%c", *sp);
				  }
				  break;
		}
		sp++;
	}
}

void P(int line, char *str) {
	gotoxy(1,(line));
	cPrintString((str));
}

static int refresh_cardio_screen = 1;

void update_screen(struct rr_t *railroad)
{
        static unsigned int loopcount = 0;
	int needmenu = 0;

	draw_cardio_screen(refresh_cardio_screen, railroad);
	needmenu = refresh_cardio_screen;
	refresh_cardio_screen=0;
        textcolor(BLACK);
	gotoxy(73,  3);
	cprintf("%6u", loopcount++);
        if (needmenu) dspmnu();
        gotoxy(menuxpos, 3);
}

void setmenu(const char far *mnu) {
	textcolor(BLACK);
	gotoxy( 3, 3);
	cprintf("%77s", "");
	gotoxy( 3, 3);
	if (mnu && *mnu) {
		menuxpos = 3 + strlen(mnu);
		menu = mnu;
        	cprintf("%s", (char *)menu);
	} else {
		menuxpos = 3;
	}
        gotoxy(menuxpos, 3);
}

void clrdspmnu(void)
{
	gotoxy( 3, 2);
	cprintf("%77s", "");
        setmenu("");
}

void dspmnu(void)
{
    gotoxy( 3, 2);
    cprintf("SBHRS CHUBB Test Harness                                              ");
    gotoxy( 3, 3);
    setmenu("C)ard P)ort 1-8 Q)uit:");
}


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
P( 4,"|                                                                              |");
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
draw_cardio_screen(int refresh, struct rr_t *railroad)
{
	int i;
	int Iline = 4;
	int line = Iline;
	int col = 2;

	int inputcard = 0;
	int outputcard = 0;

	char buf[40];

        if (refresh) screen_background();

        textcolor(BLACK);
	for (int card = 0; card < 63; card++) {
	    int ct = railroad->IOcards->Type(card);

	    if (line >= 25) {
		line = Iline;
		col += 26;
		if (col >= 80) continue;
	    }
	    gotoxy(col, line++);

	    buf[0] = '\0';
	    if (ct == 'I') {
		    textcolor(YELLOW);
		    cprintf("%02d", card);
		    textcolor(BLUE);
		    for (int pin = 24; pin >= 1; pin--) {
			int port = pin2port(pin);
			int bit  = pin2bit(pin);
			int idx;

			int offset = inputcard * 3 + 1 + port;
			if (offset > railroad->IOcards->InputBufferSize()) {
				strcat(buf,  "<");
			} else {
				idx = railroad->InBuf[offset];
				strcat(buf, (((idx >> bit) & 0x01) == 1) ? "1" : "0");
			}
		    }
		    cprintf(buf);
		    inputcard++;
	    } else if (ct == 'O') {
		    textcolor(GREEN);
		    cprintf("%02d", card);
		    textcolor(BROWN);
		    for (int pin = 24; pin >= 1; pin--) {
			int port = pin2port(pin);
			int bit  = pin2bit(pin);
			int idx;

			int offset = outputcard * 3 + 1 + port;
			if (offset > railroad->IOcards->OutputBufferSize()) {
				strcat(buf,  ">");
			} else {
				idx = railroad->OutBuf[offset];
				strcat(buf, (((idx >> bit) & 0x01) == 1) ? "1" : "0");
			}
		    }
		    cprintf(buf);
		    outputcard++;
	    } else {
		    textcolor(RED);
		    cprintf("%02d------------------------", card);
	    }
	}
        textcolor(BLACK);
	gotoxy( 3,22);
}

void Input(struct rr_t *railroad)
{
        if (railroad->usic) {
		railroad->usic->Read(railroad->InBuf);
	}
	
#ifdef DEBUG_Inputbits
	printf("Input: ");
	printf(" %s", binary(8,(unsigned long)railroad->InBuf[1]));
    	printf(" %s", binary(8,(unsigned long)railroad->InBuf[2]));
    	printf(" %s", binary(8,(unsigned long)railroad->InBuf[3]));
	printf("\n");
#endif
}

int Process(struct rr_t *railroad)
{
	update_screen(railroad);
	sleep(1);
	return 0;
}

void Output(struct rr_t *railroad)
{

#ifdef DEBUG_Outputbits
	printf("Output:");
	printf(" %s", binary(8,(unsigned long)railroad->OutBuf[1]));
    	printf(" %s", binary(8,(unsigned long)railroad->OutBuf[2]));
    	printf(" %s", binary(8,(unsigned long)railroad->OutBuf[3]));
	printf("\n");
#endif
        if (railroad->usic) {
	        railroad->usic->Write(railroad->OutBuf);
	}
}


#include "kbdio.h"

int getInputnumber(struct rr_t *railroad, int x, int y, int digits)
{
	int numdigits = 0;
	int num = 0;
	int c;

        gotoxy(x,y);
        for(;;) {
		check(railroad);
		while (kbhit()) {
			c = getch();
			if (c >= '0' && c <= '9') {
				num = num * 10 + (c - '0');
				gotoxy(x,y); cprintf("%d", num);
				numdigits++;
			} else if (c == '\n' || c == '\r') {
				return num;
			} else if (c == ' ' ) {
				return num;
			} else {
				return -1;
			}
			if (numdigits == digits) {
				return num;
			}
		}
        }
}

char* getInputstring(struct rr_t *railroad, int x, int y, int len)
{
	int done = 0;
	int pos = 0;
	char line[80];
	char *retval;
	int c;

	line[0] = '\0';
        gotoxy(x,y);
        while (!done) {
		check(railroad);
		while (kbhit()) {
			c = getch();
			if (c == '\n' || c == '\r' || c == '\026') {
				done++;
				break;
			} else if (c == '\b') {
				if (pos > 0) pos--;
			} else if ((c == ' ') ||
				   (c == '-') ||
				   (c == '[') ||
				   (c == ']') ||
				   (c == '/') ||
				   ((c >= '0') && (c <= '9')) ||
				   ((c >= 'A') && (c <= 'Z')) ||
				   ((c >= 'a') && (c <= 'z'))
				  ) {
				line[pos++] = c;
				line[pos] = '\0';
				gotoxy(x,y); cprintf("%s", line);
			}
			if (pos == 79)  { done++; break; }
			if (pos == len) { done++; break; }
		}
        }
	if (pos > 0) {
		retval = new char[pos + 1];
		strcpy(retval, line);
	} else {
		retval="";
	}
	return retval;
}


int kbdio_card(struct rr_t *railroad) {
	char menu[80];
	int card = 0, xpos = 0, error = 0;
	int ch = getch();

	if (ch >= 'a') {
	    ch -= 32;       // make it upper case
	}
	switch(ch) {
	    case 'S':
	    case 'T':
	    case 'C':
			clrdspmnu();
			gotoxy( 3, 2); cprintf("X)xxxxx: ");
			sprintf(menu, "   Card:___");
			setmenu(menu);
			xpos = strlen(menu);
			for(;;) {
				card   = getInputnumber(railroad, xpos, 3, 2);
				if (card == -1) { error = 1; break; }
				sprintf(menu, "   Card %3d is invalid (try 18-62...):___", card);
				setmenu(menu);
				xpos = 3 + strlen(menu);
			}
			if (error) {
			    sprintf(menu, "   Invalid Input");
			    setmenu(menu);
			}
			dspmnu();
		break;
	    case 'Q':	return 'Q';
	}
	return 1;
}

int kbdio(struct rr_t *railroad) {

	if (kbhit()) {
	    char menu[80];
	    int card = 0, xpos = 0, error = 0;
	    int ch = getch();

	    if (ch >= 'a') {
		ch -= 32;       // make it upper case
	    }
	    switch(ch) {
		case 'C':
			    clrdspmnu();
			    gotoxy( 3, 2); cprintf("C)ard: ");
			    sprintf(menu, "   Card:___");
			    setmenu(menu);
			    xpos = strlen(menu);
			    for(;;) {
				    card   = getInputnumber(railroad, xpos, 3, 2);
				    if ((card == -1) ||
				        (card < 18) ||
				        (card > 64)) {
				        error = 1;
					break;
				    }

			    }
			    if (error) {
				    sprintf(menu, "   Card %3d is invalid (try 18-63...):___", card);
				    setmenu(menu);
				    xpos = 3 + strlen(menu);
			    }
			    dspmnu();
		    break;
		case 'B':
		case 'Q':	return 'Q';
	    }
	}
	return 0;
}

void usage() {
        fprintf(stderr, "Error: command line malformed\n");
	exit(0);
}


int
main(int argc, char *argv[])
{
        int y, x = 0;
	int done;

	// int nochubb = FALSE;
	int nochubb = TRUE;
	int debug   = FALSE;
	int defaultport = Port::COM1;

	// check command line input
	if (--argc >= 1) {
	    for (int i = 1; i <= argc; i++) {
		if ((argv[i][0] != '/') && (argv[i][0] != '-')) //check first character
		    usage();
		int c = argv[i][1];        // get 1st valid character
		if (c >= 'a') {
		    c -= 32;       // make it upper case
		}
		switch (c)  {        // get the options
		    case 'D' : debug   = TRUE; break;
		    case 'N' : nochubb = TRUE;
			       defaultport = Port::NONE;			    break;

		    case '1' : defaultport = Port::COM1; break;
		    case '2' : defaultport = Port::COM2; break;
		    case '3' : defaultport = Port::COM3; break;
		    case '4' : defaultport = Port::COM4; break;
		    default  : usage(); break;
		}
	    }
	}
	rr_t *railroad = new rr_t();
	railroad->cardnum=18;
	railroad->portnum=0;

	setbuf (stdout, NULL);

        // textmode(3);                    // Set for 16 color
        // textcolor(BLACK);               // Text color
        // textbackground(LIGHTGRAY);      // Background color
        // clrscr();                       // Clear it

	printf("Chubb Test Program\n");
   	if (debug)
	    printf("Initializing Card Arrays\n");

	railroad->IOcards = new Cards(
		"IIII"  //  0  1  2  3	Input
		"IIII"  //  4  5  6  7	Input
		"IIII"  //  8  9 10 11	Input
		"IIII"  // 12 13 14 15	Input
		"IIOO"  // 16 17	Input
			//       18 19  Output
		"OOOO"  // 20 21 22 23	Output
		"OOOO"  // 24 25 26 27	Output
		"OOOO"  // 28 29 30 31	Output
		"OOOO"  // 32 33 34 35	Output
		"OOOO"  // 36 37 38 39	Output
		"OOOO"  // 40 41 42 43	Output
		"OOOO"  // 44 45 46 47	Output
		"OOOO"  // 48 49 50 51	Output
		"OOOO"  // 52 53 54 55	Output
		"OOOO"  // 56 57 58 59	Output
		"OOOO"  // 60 61 62 63	Output
		"");

	// railroad->IOcards->print();

   	if (debug)
	    printf("Allocating Buffers\n");

	railroad->OutBuf = new BYTE[railroad->IOcards->OutputBufferSize()];
	railroad->SavBuf = new BYTE[railroad->IOcards->OutputBufferSize()];
	railroad->InBuf  = new BYTE[railroad->IOcards->InputBufferSize()];

	for (x=0; x <= railroad->IOcards->NumberOutputPorts(); x++) {
		railroad->OutBuf[x] = 0;
		railroad->SavBuf[x] = 0;
	}
	for (x=0; x <= railroad->IOcards->NumberInputPorts(); x++) {
		railroad->InBuf[x]  = 0;
	}

   	if (debug)
	    printf("Initializing USIC\n");

        if (nochubb == FALSE) {
		railroad->usic = new USIC(railroad->IOcards, 0, defaultport, 9600);
	} else {
		railroad->usic = NULL;
	}
	
	while (1) {
		if (kbdio(railroad) == 'Q') {
		    break;
		}
		check(railroad);
	}
	gotoxy(1,24);
}

void check(struct rr_t *railroad) {
	Input(railroad);
	Process(railroad);
	Output(railroad);
}


