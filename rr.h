// @(#)rr.h	1.2 99/08/06 SBHRS

/*  This is the header file for the SBHRS layout
 *  control program.
 *
 *  This is converted from earlier versions.
 *  by LARRY KLOTH
 *    july 11, 1995
 *
 */

#ifndef _RR_H_
#define _RR_H_

#define NATIVE	/* Define if compiling on a PC with Borland C++ */

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <iostream.h>
#include <fstream.h> // for ifstream, ofstream (disk I/O)
#include <string.h>
#include <time.h>    // for time keeping

#if defined(NATIVE)
#	include <conio.h>
#	include <dos.h>
#	define MSDOS
#else	/* Unix cross environment */
#	include <stdarg.h>
#	include <unistd.h>

	extern void gotoxy(int x, int y);
	extern int getch(void);
	extern void putch(int);
	extern void cprintf(char* str, ...);
	extern void gettext (int, int, int, int, char*);
	extern void puttext (int, int, int, int, char*);
	extern void window (int, int, int, int);
	extern int textmode(int);
	extern int kbhit(void);
	extern void textcolor(int);
	extern void textbackground(int);
	extern void clrscr(void);

#	define WHITE		0
#	define BLACK		1
#	define YELLOW		2
#	define LIGHTGRAY	3
#	define BLUE		4
#	define RED		5
#	define GREEN		6
#	define BROWN		7

#endif

typedef unsigned char BYTE;

// First, setup some useful constants.

enum Boolean {FALSE, TRUE};
enum Aspect  {RR_NONE, RR_RED, RR_YELLOW, RR_GREEN};     // signal aspects


// Now, for the railroad

// holds the function prototypes for the various functions used
// by the rr.cpp program

//#define IDE   // causes delays so that computer can run IDE

void logmessage(const char *message);

void InitOutBuf(BYTE outbuf[]);
void autoad(BYTE *InBuf, BYTE *Outbuf, BYTE *SavBuf);
void BlockInit(void);
void panic(class USIC *usic);
void checkdetectors();
void checkturnouts(void);
void adjustblock(int turnoutnumber, int direction);
void CheckSubBlock(void);
void checkmanuals(void);
void throttlebutton(void);
void crossover(void);
void checkcabdirections(void);
void do_signals(void);

void assigndrop(BYTE *InBuf, BYTE *OutBuf, BYTE* SavBuf);
void dropblock(BYTE *buf, int block);
void assignblock(BYTE *buf, int block, int cab);

// keyboard
void emulate_init(BYTE *InBuf, BYTE *OutBuf, BYTE *SaveBuf); 
void emulate();
void adjustblocks(int turnoutnumber, int direction);
void staging(void);

void em_normalize(int in);
void em_reverse(int in);
void em_unlock(int in);
void em_lock(int in);
void em_toggle(int in);

// rropbfsc
void bufferscreen();
void openinitwindow(char[]);
void closeinitwindow(char[]);
void openi_owindow(char[]);
void closei_owindow(char[]);
void openi_ostart(char[]);
void closei_ostart(char[]);
void opendebugwindow(char[]);
void closedebugwindow(char[]);
void update_screen(class USIC *usic);

// rroperat
void key_assign(class USIC * usic);
void key_drop(class USIC * usic);
void key_turnout(class USIC * usic);
void key_panic(class USIC * usic);
void key_route(class USIC * usic);
void key_options(class USIC * usic);
void key_getcard(class USIC * usic);
void setmenu(const char * mnu);
void clrdspmnu(void);
void dspmnu(void);
void RRLOOP(class USIC * usic);
// BYTE zero(BYTE buffer, BYTE mask, int shift);


#endif

