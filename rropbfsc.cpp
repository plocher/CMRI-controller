// @(#)rropbfsc.cc	1.2 99/08/06 SBHRS

// Sets up the computer screen for rr i/o.  Common APIs and utility functions

#include    "rr.h"
#include    "globals.h"
#include <ctype.h>

extern void draw_debug_screen(int refresh, USIC *usic);
extern void draw_dispatch_screen(int refresh, USIC *usic);
extern void draw_log_screen(int refresh, USIC *usic);
extern void draw_cardio_screen(int refresh, USIC *usic);

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

static int refresh_dispatch_screen = 1;
static int refresh_debug_screen = 1;
static int refresh_log_screen = 1;
static int refresh_cardio_screen = 1;

void update_screen(USIC *usic)
{
        static unsigned int loopcount = 0;
	int needmenu = 0;

	if (options.debug_screen == TRUE) {
		draw_debug_screen(refresh_debug_screen, usic);
		needmenu = refresh_debug_screen;
		refresh_debug_screen = 0;
		refresh_log_screen = 1;
		refresh_cardio_screen = 1;
		refresh_dispatch_screen = 1;
	} else if (options.log_screen == TRUE) {
		draw_log_screen(refresh_log_screen, usic);
		needmenu = refresh_log_screen;
		refresh_log_screen = 0;
		refresh_cardio_screen = 1;
		refresh_debug_screen = 1;
		refresh_dispatch_screen = 1;
	} else if (options.cardio_screen == TRUE) {
		draw_cardio_screen(refresh_cardio_screen, usic);
		needmenu = refresh_cardio_screen;
		refresh_cardio_screen = 0;
		refresh_log_screen = 1;
		refresh_debug_screen = 1;
		refresh_dispatch_screen = 1;
	} else {
		draw_dispatch_screen(refresh_dispatch_screen, usic);
		needmenu = refresh_dispatch_screen;
		refresh_dispatch_screen = 0;
		refresh_log_screen = 1;
		refresh_cardio_screen = 1;
		refresh_debug_screen = 1;
	}
        textcolor(BLACK);
        if (options.showloopcounter) {
	        gotoxy(73,  3);
	        cprintf("%6u", loopcount);
        }
        loopcount++;
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
	char menustring[100];

	gotoxy( 3, 2);
	cprintf("SBHRS CTC/Dispatcher                                      Version %-10s", VERSION);
	gotoxy( 3, 3);
	strcpy(menustring, "A)ssign D)rop T)urnout P)anic R)oute O)ptions");
	if (options.nochubb == TRUE) {
	    strcat(menustring, " E)mulate");
	}
	if (options.cardio_screen == TRUE) {
	    strcat(menustring, " C)ard");
	}
	strcat(menustring, " Q)uit");
	setmenu(menustring);
}

