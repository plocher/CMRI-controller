// @(#)crossenv.cc	1.2 99/08/06 SBHRS

#include "rr.h"

#ifndef NATIVE


#define _XOPEN_SOURCE
#undef _XOPEN_VERSION
#define _XOPEN_VERSION	4

#include <curses.h>
#include <stdlib.h>
#include <stdarg.h>

static WINDOW *oldwindow[10];
static int sp = 0;

static void uninit_windows(void)
{
	endwin();	
}

static void init_windows()
{
	static int done = 0;

	if (!done) {
		done=1;
		sp = 0;
		oldwindow[sp] = initscr();
		atexit(uninit_windows);
		cbreak();
		noecho();
		nonl();
		intrflush(oldwindow[sp], FALSE);
		keypad(oldwindow[sp], TRUE);
	}
}


void gotoxy(int x, int y)
{
	init_windows();
	wmove(oldwindow[sp], y, x-1);
}

#undef getch

int getch(void)
{
	int ch;

	init_windows();
	ch =  wgetch(oldwindow[sp]);
	waddch(oldwindow[sp], ch);
	wrefresh(oldwindow[sp]);
	return ch;
}

void putch(int ch)
{
	init_windows();
	waddch(oldwindow[sp], ch);
	wrefresh(oldwindow[sp]);
}

void cprintf(char* str, ...)
{
	va_list ap;
	char buffer[1024];

	va_start(ap, str);

	init_windows();
	vsprintf(buffer, str, ap);
	waddstr(oldwindow[sp], buffer);
	wrefresh(oldwindow[sp]);

	va_end(ap);
}

void gettext (int, int, int, int, char*)
{
	init_windows();
}

void puttext (int, int, int, int, char*)
{
	init_windows();
	touchwin(oldwindow[sp]);
	wrefresh(oldwindow[sp]);
}

void window (int x1, int y1, int x2, int y2)
{


	init_windows();
	if ((x1 == 1) && (y1 == 1) && (x2==80) && (y2 == 25)) {
		while (sp > 0) {
			delwin(oldwindow[sp]);
			sp--;
		}
		touchwin(oldwindow[sp]);
		wrefresh(oldwindow[sp]);
	} else {
		// Original, has off by 1 errors
		// oldwindow[++sp] = newwin(y2-y1, x2-x1, y1-1, x1-1);
		oldwindow[++sp] = newwin(y2-y1+1, x2-x1+1, y1, x1);
	}
}

int  textmode(int)
{
	init_windows();
	return 0;
}

int  kbhit(void)
{
	init_windows();
	return 0;
}

void textcolor(int)
{
	init_windows();
}

void textbackground(int)
{
	init_windows();
}

void clrscr(void)
{
	init_windows();
	werase(oldwindow[sp]);
	wrefresh(oldwindow[sp]);
}

void outp(uint32_t, int)
{
}

int  inp(uint32_t)
{
	return 0;
}

#endif


