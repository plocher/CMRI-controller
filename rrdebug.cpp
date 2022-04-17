
// @(#)rrdebug.cc	1.2 99/08/06 SBHRS

#include    "rr.h"
#include    "globals.h"
#include "panel.h"
#include <ctype.h>

extern void P(int line, char *str);

static void
debug_screen_background(void)
{
    int i;

    textbackground(LIGHTGRAY);
    clrscr();
//	       1        2         3        4         5        6         7
//    1234567890123456780123456789012345678012345678901234567801234567890123456789012
P( 1,"+------------------------------------------------------------------------------+");
P( 2,"| TITLE goes here (See dspmnu() for details)                                   |");
P( 3,"| Commands menu goes here                                                      |");
P( 4,"+------------------------------------------------------------------------------+");
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
draw_debug_screen(int refresh, USIC *usic)
{
	int i;
	int line = 5;

        if (refresh) debug_screen_background();

        textcolor(BLACK);
        for(i = 1; i <= NumberBlocks; i++) {
                if ((i % 10) == 0) {
			gotoxy( 7+i, line);
			cprintf("%d", i / 10);
		}
        }
	gotoxy( 8, ++line);
        for(i = 1; i <= NumberBlocks; i++) {
                cprintf("%d", i % 10);
        }
	gotoxy( 3, ++line);
	cprintf("Blk: ");
        textcolor(BLACK);
        for(i = 1; i <= NumberBlocks; i++) {
	    int port, bit;
            port = blockbuf[i][DETBUF];
            bit  = blockbuf[i][DETS];
            int det = (int)((InBuf[port] >> bit) & 0x01);
	    if (det) {
		    textcolor(WHITE);
		    cprintf("*");
		    textcolor(BLACK);
	    } else {
		    cprintf(".");
	    }
        }

	gotoxy( 3, ++line);
	cprintf("Cab: ");
        textcolor(BLACK);
	for(i = 1; i <= NumberBlocks; i++) {
		int currentcab = blockbuf[i][CURCAB];
		if (currentcab == NOCAB) {
			cprintf(".");
		} else if (currentcab == DCC) {
			cprintf("D");
		} else {
			cprintf("%d", currentcab);
		}
	}

	gotoxy( 3, ++line);
	cprintf("CRC: ");
        textcolor(BLACK);
	for(i = 1; i <= NumberBlocks; i++) {
		int currentcab;
		currentcab = ((OutBuf)[blockbuf[i][OBUF]] >> blockbuf[i][OBUFS]) & 0x07;
		currentcab ^= 0x07;
		if (currentcab == NOCAB) {
			cprintf(".");
		} else if (currentcab == DCC) {
			cprintf("D");
		} else {
			cprintf("%d", currentcab);
		}
	}

	gotoxy( 3, ++line);
	cprintf("Lst: ");
        textcolor(RED);
	for(i = 1; i <= NumberBlocks; i++) {
		int lastcab = blockbuf[i][LASTCAB];
		if (lastcab == NOCAB) {
			cprintf(".");
		} else if (lastcab == DCC) {
			cprintf("D");
		} else {
			cprintf("%d", lastcab);
		}
	}

	gotoxy( 3, ++line);
	cprintf("Wsg: ");
        textcolor(BLACK);
	for (i = 1; i < NumberBlocks + 1; i++) {
		int color, red, yel, grn;
		char head;
		if (blockbuf[i][WSIGBASE] == 0) {
		        color = BLACK;
			head = '.';
		} else {
			red = OutBuf[blockbuf[i][WSIGBASE]+0] & (1 << blockbuf[i][WSIGOFF]);
			yel = OutBuf[blockbuf[i][WSIGBASE]+1] & (1 << blockbuf[i][WSIGOFF]);
			grn = OutBuf[blockbuf[i][WSIGBASE]+2] & (1 << blockbuf[i][WSIGOFF]);

			if      (red) color = RED; 
			else if (yel) color = YELLOW;
			else if (grn) color = GREEN;
			else color = BLACK;
			head = 'o';
		}
	        gotoxy( 7 + i,line); textcolor(color); cprintf("%c", head);
	}
	gotoxy( 3,++line);
	cprintf("Esg: ");
        textcolor(BLACK);
	for (i = 1; i < NumberBlocks + 1; i++) {
		int color, red, yel, grn;
		char head;
		if (blockbuf[i][ESIGBASE] == 0) {
		        color = BLACK;
			head = '.';
		} else {
			red = OutBuf[blockbuf[i][ESIGBASE]+0] & (1 << blockbuf[i][ESIGOFF]);
			yel = OutBuf[blockbuf[i][ESIGBASE]+1] & (1 << blockbuf[i][ESIGOFF]);
			grn = OutBuf[blockbuf[i][ESIGBASE]+2] & (1 << blockbuf[i][ESIGOFF]);

			if      (red) color = RED; 
			else if (yel) color = YELLOW;
			else if (grn) color = GREEN;
			else color = BLACK;
			head = 'o';
		}
	        gotoxy( 7 + i,line); textcolor(color); cprintf("%c", head);
	}

	gotoxy( 3, ++line);
	cprintf("Sub: ");
        textcolor(BLACK);
        for(i = 1; i <= NumberSubs; i++) {
	    int port, bit;
            port = subblockbuf[i][0];
            bit  = subblockbuf[i][1];
            int det = (int)((InBuf[port] >> bit) & 0x01);
	    if (det) {
		    textcolor(WHITE);
		    cprintf("*");
		    textcolor(BLACK);
	    } else {
		    cprintf(".");
	    }
        }

        textcolor(BLACK);
	gotoxy( 8, ++line);
        for(i = 1; i <= NumberTurnouts; i++) {
                if ((i % 10) == 0) {
			gotoxy( 7+i, line);
			cprintf("%d", i / 10);
		}
        }
	gotoxy( 8, ++line);
        for(i = 1; i <= NumberTurnouts; i++) {
                cprintf("%d", i % 10);
        }
	gotoxy( 3,++line);
	cprintf("Lck: ");
	gotoxy( 3,++line);
	cprintf("Tur: ");
	gotoxy( 3,++line);
	cprintf("Mot: ");
        for(i = 1; i <= NumberTurnouts; i++) {
	    char m = ' ', p = ' ', c = ' ';
	    int mcolor = BLACK;
	    int pcolor = BLACK;
	    int ccolor = BLACK;
            int man = InBuf[turnout[i][MANUAL_PORT]]   & (0x01 << turnout[i][MANUAL_BITS]);
            int pos = InBuf[turnout[i][POSITION_PORT]] & (0x01 << turnout[i][POSITION_BITS]);
	    int mot = OutBuf[turnout[i][CONTROL_PORT]] & (0x01 << turnout[i][CONTROL_BITS]);
            if (man != 0) { mcolor = BLACK; m = '.'; } else { mcolor = RED;   m = 'U'; }
            if (pos != 0) { pcolor = RED;   p = 'D'; } else { pcolor = GREEN; p = 'N'; }
            if (mot != 0) { ccolor = RED;   c = 'd'; } else { ccolor = GREEN; c = 'n'; }
	    gotoxy( 7 + i,line-2); textcolor(mcolor); cprintf("%c", m);
	    gotoxy( 7 + i,line-1); textcolor(pcolor); cprintf("%c", p);
	    gotoxy( 7 + i,line  ); textcolor(ccolor); cprintf("%c", c);
        }


	textcolor(BLACK);
	gotoxy( 8, ++line);
        for(i = 1; i <= NumberCabs; i++) {
                cprintf("%d", i % 10);
        }
	gotoxy( 3,++line); cprintf("Dir: ");
	gotoxy( 3,++line); cprintf("But: ");
	gotoxy( 3,++line); cprintf("LED: ");
        for(i = 1; i <= NumberCabs; i++) {
	        int port, bit;
		char d, p, l;
		int dcolor, pcolor, lcolor;

		port = throttle[i][REVSPORT];
		bit  = throttle[i][REVSOFF];
		if (InBuf  [port] & (0x01 << bit)) { dcolor = GREEN; d = 'E'; }
		else                               { dcolor = RED;   d = 'W'; }

		port = throttle[i][PB_PORT];
		bit  = throttle[i][PB_OFF];
		if (InBuf  [port] & (0x01 << bit)) { pcolor = WHITE; p = '*'; }
		else                               { pcolor = BLACK; p = ' '; }

		port = throttle[i][LED_PORT];
		bit  = throttle[i][LED_OFF];
		if (OutBuf [port] & (0x01 << bit)) { lcolor = RED;   l = '*'; }
		else                               { lcolor = BLACK; l = ' '; }

	        gotoxy( 7 + i,line-2); textcolor(dcolor); cprintf("%c", d);
	        gotoxy( 7 + i,line-1); textcolor(pcolor); cprintf("%c", p);
	        gotoxy( 7 + i,line-0); textcolor(lcolor); cprintf("%c", l);
	}

        textcolor(BLACK);
	gotoxy( 8,23); cprintf("Panel 1  Panel 2  Panel 3  Panel 4  Panel 5  Panel 6\n");
	gotoxy( 8,24);
	for (i = 1; i <= 6; i++) {
	    // Get panel settings
	    int mainline = PANEL_GET_MAIN(InBuf, panel[i]);
	    int block    = PANEL_GET_BLOCK(InBuf, panel[i]);
	    int cab      = PANEL_GET_CAB(InBuf, panel[i]);
	    int drop     = InBuf[PANEL_DROP] & panel[i][PANEL_BUTTON];
	    int assign   = InBuf[PANEL_ASSIGN] & panel[i][PANEL_BUTTON];

	    if (mainline + block + cab + drop + assign == 0) {
		    textcolor(BLACK);
		    cprintf("- n/c -");
	    } else {
		    textcolor(RED);
		    if (drop) cprintf("D"); else cprintf(" ");
		    textcolor(BLACK);
		    cprintf("%1d%2d:%1d ", mainline, block, cab);
		    textcolor(RED);
		    if (assign) cprintf("A"); else cprintf(" ");
	    }
	    cprintf("  ");
        }
        textcolor(BLACK);
	gotoxy( 3,22);
}

