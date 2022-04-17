// @(#)rrlog.cc	1.2 99/08/06 SBHRS

#include    "rr.h"
#include    "globals.h"
#include    "panel.h"

#include <ctype.h>

extern void P(int line, char *str);

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
draw_log_screen(int refresh, USIC *usic)
{
	int i;
	int line;

        if (refresh) screen_background();

        textcolor(BLACK);
	if (logQ->head == logQ->tail) {
	        line = 5;
		gotoxy( 3, line);
		cprintf("No Messages [modified= %d lastmod= %d head=0x%lx tail=0x%lx",
				logQ->modified, logQ->lastmod, logQ->head, logQ->tail);
	} else {
	    LogQ::Message *m;
	    if (logQ->modified) {
		    logQ->lastmod = logQ->modified;
		    logQ->modified = 0;
	    }
	    gotoxy( 3, 5);
	    cprintf("%d Messages [modified= %d lastmod= %d head=0x%lx tail=0x%lx",
			logQ->counter, logQ->modified, logQ->lastmod, logQ->head, logQ->tail);
	    line = 24;
	    int mline = logQ->lastmod;
	    for (m = logQ->head; m; m = m->p) { 
		if (line == 5) break;
		gotoxy( 3, line);
		cprintf("                                                                          ");
		gotoxy( 3, line);
        	if (mline > 0) {
			textcolor(RED);
		} else {
			textcolor(BLACK);
		}
		cprintf(" %s", m->text);

		line--;
		mline--;
	    }
	}
        textcolor(BLACK);
	gotoxy( 3,22);
}

