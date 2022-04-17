
// @(#)rrdisp.cc	1.2 99/08/06 SBHRS

#include    "rr.h"
#include    "globals.h"
#include <ctype.h>

#define TRACKCHAR	(char)205

#define TP(turn) turnout[(turn)][POSITION]
#define TM(turn) turnout[(turn)][LOCK]

#define T1	0
#define T2	24
#define T3	48
#define T4	64


#define TURNOUT_WB_FACING_RH	1	
#define TURNOUT_WB_TRAILING_RH	2	
#define TURNOUT_WB_FACING_LH	3	
#define TURNOUT_WB_TRAILING_LH	4	
#define TURNOUT_EB_FACING_RH	5	
#define TURNOUT_EB_TRAILING_RH	6	
#define TURNOUT_EB_FACING_LH	7	
#define TURNOUT_EB_TRAILING_LH	8	
#define TURNOUT_X_FACING_LH	9	
#define TURNOUT_X_TRAILING_RH   10	
#define TURNOUT_X_FACING_RH	11	
#define TURNOUT_X_TRAILING_LH   12	

#define SIGNAL_EAST		0x100
#define SIGNAL_WEST		0x200
#define SIGNAL_SEARCHLIGHT	0x001

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
P( 3,"| Commands menu goes here                                                 loop |");
P( 4,"+------------------------------------------------------------------------------+");
P( 5,"| CAB 1:          2:          3:          4:          5:          6:           |");
P( 6,"+------------------------------------------------------------------------------+");
P( 7,"|      3/o-   4\\ o-   7/                    Los Perdidos                       |");
P( 8,"|      +===*#===+====++==+==                   12 13     15    Quarry          |");
P( 9,"|   @2/o-         x6/o- x8\\ o- @   @   @    o- /o-\\ o- o- \\ o-@   @            |");
P(10,"|  <=+=====*#======+=======+==<=*#<=*#<====+==+=*#=+==+====+<=*#=<=*#+=====<   |");
P(11,"|         b01   x5/       x10\\ b02 b03    /x11 b04     \\x14   b05 b06 \\x28     |");
P(12,"|  >=+=====*#====+======+=====>=*#>=*#>==+======*#======+===>=*#+>=*#==+===>   |");
P(13,"|  -o1\\        -o    -o/9    @   @   @ -o                  @ -o/@16       @    |");
P(14,"|      ====*#==========                       =11=#-18-#     Comber            |");
P(15,"|         South Bay                 =02=#=03=#-04-#-17-#=07=#=08=#             |");
P(16,"|                                West Staging -12-#-16-# East Staging          |");
P(17,"|                                             -13-#-15-#                       |");
P(18,"|                         Sequoia             -14-#=06=#                       |");
P(19,"| Modules      Oil         ==*#=             Tuolumne     Lower Sequoia        |");
P(20,"|  @      o- @    @   @ 32/o- 31\\ o-@   /o-@     o-   @   @     o- @   o-      |");
P(21,"| <==*#==+==<=*#=<=*#<=*#+=======+=<=*#+==<==*#=+====<=*#<=*#==+==<=*#+====<   |");
P(22,"|   b07 /x27 b08  b09 b10   b11     b12     b13  \\x25 b14 b15 /x23 b16 \\x24    |");
P(23,"| >==*#+====>=*#+>=*#>=*#==>=*#====>=*#===>==*#===++=>=*#>=*#+====>=*#==++=>   |");
P(24,"|    -o    @  -o@\\  @     @       @      @      -o @\\26 @  -o    @    -o  \\@29  |");
P(25,"+------------------------------------------------------------------------------");

    // corners
    gotoxy( 1, 1); putch((char)218); // |--
    gotoxy(80, 1); putch((char)191); // --|
    gotoxy( 1,25); putch((char)192); // |__
 // gotoxy(80,25); putch((char)188); // __|  188	scrolls screen!

    gotoxy( 1, 4); putch((char)195); // |-
    gotoxy(80, 4); putch((char)180); // -|
    gotoxy( 1, 6); putch((char)195);
    gotoxy(80, 6); putch((char)180);
}


static void
display_switchstand(int x, int y, int type, int aspect)
{
	char	switchstandhead = (char)4; // 249;
	char	switchstandbody;
	int	switchstandcolor = aspect;
	int	switchstandbodycolor = BLACK;
	int	switchstandbodyx;

	if (type & SIGNAL_EAST) {
		switchstandbodyx = x - 1;
		switchstandbody  = '-';
	} else {
		switchstandbodyx = x + 1;
		switchstandbody  = '-';
	}

	if (options.showswitchstands == FALSE) {
		switchstandcolor = BLACK;
		switchstandhead=' ';
		switchstandbody=' ';
	}

	gotoxy(x, y);
	textcolor(switchstandcolor);
	cprintf("%c", switchstandhead);
	gotoxy(switchstandbodyx, y);
	textcolor(switchstandbodycolor);
	cprintf("%c", switchstandbody);
}
    					

static void
display_turnout(int x, int y, int type, int pos, int lock, int withsig)
{
	int	turnoutcolor;
	int	divergingcolor;
	char	turnoutchar;
	int	facingsignalcolor;
	int	trailingsignalcolor;

	if        ((pos == NORMAL) && (lock == LOCKED)) { // CTC normal
		turnoutcolor = GREEN;
		divergingcolor = BLACK;
		facingsignalcolor = GREEN;
		trailingsignalcolor = GREEN;
		turnoutchar = TRACKCHAR;
	} else if ((pos == REVERSE) && (lock == LOCKED)) { // CTC reverse
		turnoutcolor = YELLOW;
		divergingcolor = YELLOW;
		facingsignalcolor = YELLOW;
		trailingsignalcolor = RED;
		turnoutchar = '+';
	} else if ((pos == REVERSE) && (lock == UNLOCKED)) { // manual reverse
		turnoutcolor = RED;    		
		divergingcolor = RED;
		facingsignalcolor = YELLOW;
		trailingsignalcolor = RED;
		turnoutchar = '+';
	} else if ((pos == NORMAL) && (lock == UNLOCKED)) { // manual normal
		turnoutcolor = RED;
		divergingcolor = BLACK;
		facingsignalcolor = YELLOW;
		trailingsignalcolor = YELLOW;
		turnoutchar = TRACKCHAR;
	}

#define WBRH	(char)207
#define EBRH	(char)209
#define WBLH	EBRH
#define EBLH	WBRH

	switch (type) {
     					//  \ o-     \ o- 
	case TURNOUT_WB_FACING_RH:	// <-+--------\----
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBRH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x - 1, y - 1);
		cprintf("%c", '\\');

		if (withsig) {
			display_switchstand(x + 1, y - 1, SIGNAL_WEST, facingsignalcolor);
		}
		break;
					//    /o- 
	case TURNOUT_WB_TRAILING_RH:	// <-+----
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBRH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x + 1, y - 1);
		cprintf("%c", '/');

		if (withsig) {
			display_switchstand(x + 2, y - 1, SIGNAL_WEST, trailingsignalcolor);
		}
		break;

					//    o- 
	case TURNOUT_WB_FACING_LH:	// <-+----
    					//  /
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
			cprintf("%c", WBLH);
		} else {
			cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x - 1, y + 1);
		cprintf("%c", '/');

		if (withsig) {
			display_switchstand(x + 1, y - 1, SIGNAL_WEST, facingsignalcolor);
		}
		break;

    					//    o- 
	case TURNOUT_WB_TRAILING_LH:	// <-+----
    					//    \ 
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
			cprintf("%c", WBLH);
		} else {
			cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x + 1, y + 1);
		cprintf("%c", '\\');

		if (withsig) {
			display_switchstand(x + 1, y - 1, SIGNAL_WEST, trailingsignalcolor);
		}
		break;


	case TURNOUT_EB_FACING_RH:	// ----+->
    					//   -o \ 
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
			cprintf("%c", EBRH);
		} else {
			cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x + 1, y + 1);
		cprintf("%c", '\\');

		if (withsig) {
			display_switchstand(x - 1, y + 1, SIGNAL_EAST, facingsignalcolor);
		}
		break;

	case TURNOUT_EB_TRAILING_RH:	// ----+->
    					//  -o/ 
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
			cprintf("%c", EBRH);
		} else {
			cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x - 1, y + 1);
		cprintf("%c", '/');

		if (withsig) {
			display_switchstand(x - 2, y + 1, SIGNAL_EAST, trailingsignalcolor);
		}
		break;

    					//      / 
	case TURNOUT_EB_FACING_LH:	// ----+->
					//   -o
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", EBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x + 1, y - 1);
		cprintf("%c", '/');

		if (withsig) {
			display_switchstand(x - 1, y + 1, SIGNAL_EAST, facingsignalcolor);
		}
		break;

    					//    \  
	case TURNOUT_EB_TRAILING_LH:	// ----+->
    					//   -o
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", EBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x - 1, y - 1);
		cprintf("%c", '\\');

		if (withsig) {
			display_switchstand(x - 1, y + 1, SIGNAL_EAST, trailingsignalcolor);
		}
		break;

					//        o- 
					//     <-*----
    					//      /
	case TURNOUT_X_FACING_LH:	// ----+->
    					//   -o
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}
		gotoxy(x - 2, y + 2);
		if (turnoutchar == '+') {
		 	cprintf("%c", EBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x - 1, y + 1);
		cprintf("%c", '/');

		if (withsig) {
			display_switchstand(x + 1, y - 1, SIGNAL_WEST, facingsignalcolor);
			display_switchstand(x - 3, y + 3, SIGNAL_EAST, facingsignalcolor);
		}
		break;

					//    o- 
					// <-*----
    					//    \  
	case TURNOUT_X_TRAILING_LH:	// ----+->
    					//   -o
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}
		gotoxy(x + 2, y + 2);
		if (turnoutchar == '+') {
		 	cprintf("%c", EBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x + 1, y + 1);
		cprintf("%c", '\\');

		if (withsig) {
			display_switchstand(x + 1, y - 1, SIGNAL_WEST, trailingsignalcolor);
			display_switchstand(x + 1, y + 3, SIGNAL_EAST, trailingsignalcolor);
		}
		break;

    					//    
					// ----+-
    					//      \ o-
					//     <-*----
	case TURNOUT_X_FACING_RH:
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBRH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}
		gotoxy(x - 2, y - 2);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x - 1, y - 1);
		cprintf("%c", '\\');

		if (withsig) {
			display_switchstand(x + 1, y - 1, SIGNAL_WEST, facingsignalcolor);
			// display_switchstand(x - 3, y + 3, SIGNAL_EAST, facingsignalcolor);
		}
		break;

					//      -+----
    					//      /o-
	case TURNOUT_X_TRAILING_RH:	// <---*- 
		textcolor(turnoutcolor);
		gotoxy(x,y);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBRH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}
		gotoxy(x + 2, y - 2);
		if (turnoutchar == '+') {
		 	cprintf("%c", WBLH);
		} else {
		 	cprintf("%c", TRACKCHAR);
		}

		textcolor(divergingcolor);
		gotoxy(x + 1, y - 1);
		cprintf("%c", '/');

		if (withsig) {
			display_switchstand(x + 2, y - 1, SIGNAL_WEST, trailingsignalcolor);
			// display_switchstand(x - 4, y + 3, SIGNAL_EAST, trailingsignalcolor);
		}
		break;
	}
}

static void
display_block(int x, int y, int ofs, int block, char nocab, int *detector, int sx, int sy)
{
        gotoxy(x, y);
        if (detector[ofs + block] == 1) {
	    textcolor(WHITE);
	    cprintf("%c", (char)219);
	} else {
	    textcolor(BLACK);
	    cprintf("%c", (char)249);
	}
        gotoxy(x + 1, y);
        if (blockbuf[ofs + block][CURCAB] > NOCAB) {
		textcolor(BLUE);
		cprintf("%d", blockbuf[ofs + block][CURCAB]);
		textcolor(BLACK);
	} else if (blockbuf[ofs + block][LASTCAB] > NOCAB) {
		textcolor(RED);
		cprintf("%d", blockbuf[ofs + block][LASTCAB]);
		textcolor(BLACK);
	} else {
		textcolor(BLACK);
		cprintf("%c", nocab);
	}
	if (sx == 0) return;
	int i = ofs + block;
        gotoxy(sx, sy);
	if (sy < y) { // 200 main...
		if (OutBuf[blockbuf[i][ESIGBASE]] & (1 << blockbuf[i][ESIGOFF])) {
			textcolor(RED);
		} else if (OutBuf[blockbuf[i][ESIGBASE] + 1] & (1 << blockbuf[i][ESIGOFF])) {
			textcolor(YELLOW);
		} else if (OutBuf[blockbuf[i][ESIGBASE] + 2] & (1 << blockbuf[i][ESIGOFF])) {
			textcolor(GREEN);
		}
	} else { // 100 main
		if (OutBuf[blockbuf[i][WSIGBASE]] & (1 << blockbuf[i][WSIGOFF])) {
			textcolor(RED);
		} else if (OutBuf[blockbuf[i][WSIGBASE] + 1] & (1 << blockbuf[i][WSIGOFF])) {
			textcolor(YELLOW);
		} else if (OutBuf[blockbuf[i][WSIGBASE] + 2] & (1 << blockbuf[i][WSIGOFF])) {
			textcolor(GREEN);
		}
	}
	if (options.showblocksignals) {
		cprintf("%c", (char) 232);
	} else {
		cprintf(" ");
	}
}

#define SubBlockOccupied(i) (((InBuf[subblockbuf[(i)][0]] >> subblockbuf[(i)][1]) & 1) == 1)
static void
display_staging(int x, int y,
		int ofs,
		int block, int sblockE, int sblockW,
		char nocab,
		int *detector,
		int route_in,
		int route_out)
{
	int b = block;
	if (b > 9) b -= 2;	// block 9 & 10 don't exist
	gotoxy(x, y);

	if (options.showunderground) {
	        int port, bit;
                port = blockbuf[ofs + b][DETBUF];
                bit  = blockbuf[ofs + b][DETS];
                int det = (int)((InBuf[port] >> bit) & 0x01);

		if (SubBlockOccupied(sblockW)) textcolor(WHITE); else textcolor(BLACK);
		if (route_in)  cprintf("%c", TRACKCHAR); else cprintf("%c", (char)249);
		
		// if (detector[ofs + b] == 1) {
		if (det == 1) {
		    textcolor(WHITE);
		} else {
		    textcolor(BLACK);
		}
		cprintf("%02d", block);

		if (SubBlockOccupied(sblockE)) textcolor(WHITE); else textcolor(BLACK);
		if (route_out) cprintf("%c", TRACKCHAR); else cprintf("%c", (char)249);

		if (blockbuf[ofs + b][CURCAB] > NOCAB) {
			textcolor(BLUE);
			cprintf("%d", blockbuf[ofs + b][CURCAB]);
			textcolor(BLACK);
		} else {
			textcolor(BLACK);
			cprintf("%c", nocab);
		}
	} else {
		textcolor(BLACK);
		cprintf("     ");
	}
}

void draw_dispatch_screen(int refresh, USIC *usic)
{
    int i, j, detector[NumberBlocks + 1];
    int	turnoutcolor, switchstandcolor;

    for(i = 1; i <= NumberBlocks; i++) {
        detector[i] = (int)((InBuf[blockbuf[i][5]] >> blockbuf[i][6])&0x01);
    }

    if (refresh) screen_background();

    // display engine numbers

    for ( i = 1; i <= 6; i++) {
        switch (i) {
            case 1: gotoxy( 9,5); break;
            case 2: gotoxy(21,5); break;
            case 3: gotoxy(33,5); break;
            case 4: gotoxy(45,5); break;
            case 5: gotoxy(57,5); break;
            case 6: gotoxy(69,5); break;
        }
        cprintf("%10s", cabnum[i]);
    }
 
    // display block occupancy
    // Showmains
    display_block(12, 8, T2, 17, TRACKCHAR, detector, 0, 0);
    display_block(12,14, T1, 17, TRACKCHAR, detector, 0, 0);

    display_block(12,10, T2,  1, TRACKCHAR, detector, 5, 9);
    display_block(12,12, T1,  1, TRACKCHAR, detector,30,13);

    display_block(33,10, T2,  2, TRACKCHAR, detector,32, 9);
    display_block(33,12, T1,  2, TRACKCHAR, detector,34,13);

    display_block(37,10, T2,  3, TRACKCHAR, detector,36, 9);
    display_block(37,12, T1,  3, TRACKCHAR, detector,38,13);

    display_block(49,10, T2,  4, TRACKCHAR, detector,40, 9);
    display_block(49,12, T1,  4, TRACKCHAR, detector,60,13);

    display_block(63,10, T2,  5, TRACKCHAR, detector,63, 9);
    display_block(63,12, T1,  5, TRACKCHAR, detector,65,13);

    display_block(68,10, T2,  6, TRACKCHAR, detector,67, 9);
    display_block(68,12, T1,  6, TRACKCHAR, detector,75,13);

    display_block( 6,21, T2,  7, TRACKCHAR, detector, 4,20);
    display_block( 6,23, T1,  7, TRACKCHAR, detector,12,24);

    display_block(15,21, T2,  8, TRACKCHAR, detector,14,20);
    display_block(15,23, T1,  8, TRACKCHAR, detector,17,24);

    display_block(20,21, T2,  9, TRACKCHAR, detector,19,20);
    display_block(20,23, T1,  9, TRACKCHAR, detector,21,24);

    display_block(24,21, T2, 10, TRACKCHAR, detector,23,20);
    display_block(24,23, T1, 10, TRACKCHAR, detector,27,24);

    // There is no block 11 on Track2
    display_block(30,23, T1, 11, TRACKCHAR, detector,35,24);

    display_block(38,21, T2, 12, TRACKCHAR, detector,37,20);
    display_block(38,23, T1, 12, TRACKCHAR, detector,42,24);

    display_block(46,21, T2, 13, TRACKCHAR, detector,44,20);
    display_block(46,23, T1, 13, TRACKCHAR, detector,52,24);

    display_block(56,21, T2, 14, TRACKCHAR, detector,55,20);
    display_block(56,23, T1, 14, TRACKCHAR, detector,57,24);

    display_block(60,21, T2, 15, TRACKCHAR, detector,59,20);
    display_block(60,23, T1, 15, TRACKCHAR, detector,66,24);

    display_block(69,21, T2, 16, TRACKCHAR, detector,68,20);
    display_block(69,23, T1, 16, TRACKCHAR, detector,76,24);

    display_block(30,19, T2, 22, TRACKCHAR, detector, 0, 0);

    textcolor(BROWN);
    gotoxy(29+5,16); if (options.showunderground) cprintf("West Staging");
	           else				cprintf("            ");
    gotoxy(58,16); if (options.showunderground) cprintf("East Staging");
	           else				cprintf("            ");
    textcolor(BLACK);

    display_staging(32+5,15, T3,  2,  0,  0, TRACKCHAR, detector,
		    					TRUE, TRUE);
    display_staging(37+5,15, T3,  3,  0,  0, TRACKCHAR, detector,
		    					TRUE, TRUE);
    display_staging(42+5,14, T3, 11, 15, 16, TRACKCHAR,       detector,
		    					(TP(45) == REVERSE),
		    					TRUE);
    display_staging(42+5,15, T3,  4, 11, 12, TRACKCHAR,       detector,
		    					(TP(45) == NORMAL) &&
		    					(TP(46) == NORMAL),
							TRUE);
    display_staging(42+5,16, T3, 12, 17, 18, TRACKCHAR,       detector,
		    					(TP(45) == NORMAL) &&
		    					(TP(46) == REVERSE) &&
							(TP(47) == NORMAL),
							TRUE);
    display_staging(42+5,17, T3, 13, 19, 20, TRACKCHAR,       detector,
		    					(TP(45) == NORMAL) &&
		    					(TP(46) == REVERSE) &&
							(TP(47) == REVERSE) &&
							(TP(48) == NORMAL),
							TRUE);
    display_staging(42+5,18, T3, 14, 21, 22, TRACKCHAR,       detector,
		    					(TP(45) == NORMAL) &&
		    					(TP(46) == REVERSE) &&
							(TP(47) == REVERSE) &&
							(TP(48) == REVERSE),
							TRUE);
    // display_staging(47,15, T3,  5,  0,  0, TRACKCHAR, detector, TRUE, TRUE);
    // 29 27 25 23 13
    display_staging(52,14, T3, 18, 16, 30, TRACKCHAR,       detector,
		    					TRUE,
		    					(TP(33) == NORMAL));
    display_staging(52,15, T3, 17, 12, 28, TRACKCHAR,       detector,
		    					TRUE,
							(TP(33) == REVERSE) &&
							(TP(34) == NORMAL));
    display_staging(52,16, T3, 16, 18, 26, TRACKCHAR,       detector,
		    					TRUE,
							(TP(33) == REVERSE) &&
							(TP(34) == REVERSE) &&
							(TP(35) == NORMAL));
    display_staging(52,17, T3, 15, 20, 24, TRACKCHAR,       detector,
		    					TRUE,
							(TP(33) == REVERSE) &&
							(TP(34) == REVERSE) &&
							(TP(35) == REVERSE) &&
							(TP(36) == NORMAL));
    display_staging(52,18, T3,  6, 22, 14, TRACKCHAR,       detector,
		    					TRUE,
							(TP(33) == REVERSE) &&
							(TP(34) == REVERSE) &&
							(TP(35) == REVERSE) &&
							(TP(36) == REVERSE));
    display_staging(57,15, T3,  7,  0,  0, TRACKCHAR, detector,
		    					TRUE, TRUE);
    display_staging(62,15, T3,  8,  0,  0, TRACKCHAR, detector,
		    					TRUE, TRUE);

    // Display turnout status
    display_turnout( 6, 12, TURNOUT_EB_FACING_RH,   TP( 1), TM( 1), 1);
    display_turnout( 6, 10, TURNOUT_WB_TRAILING_RH, TP( 2), TM( 2), 1);
    display_turnout( 8,  8, TURNOUT_WB_TRAILING_RH, TP( 3), TM( 3), 1);
    display_turnout(17,  8, TURNOUT_WB_FACING_RH,   TP( 4), TM( 4), 1);
    display_turnout(20, 10, TURNOUT_X_FACING_LH,    TP( 5), TM( 5), 1);
    display_turnout(22,  8, TURNOUT_X_FACING_LH,    TP( 6), TM( 6), 0);

    //
    // Double slip #5 to double slip #6 xover
    // special case the switchstand...
    //
    if (TP(6)) {
	    switchstandcolor = RED;
	    turnoutcolor = RED;
    } else if (TM(6) || TM(5)) {
	    switchstandcolor = YELLOW;
	    turnoutcolor = RED;
    } else {
	    switchstandcolor = GREEN;
	    turnoutcolor = GREEN;
    }
    display_switchstand (20 + 2,10 - 1, SIGNAL_WEST, switchstandcolor);
    //
    // ... and the shared "points" indication
    //
    textcolor(turnoutcolor);
    if (TP(5) && TP(6)) {
	    gotoxy(20, 10); cprintf("%c", (char)216);
    } else if (TP(5) && !TP(6)) {
	    gotoxy(20, 10); cprintf("%c", (char)209);
    } else if (!TP(5) && TP(6)) {
	    gotoxy(20, 10); cprintf("%c", (char)207);
    }
    textcolor(BLACK);

    display_turnout(23,  8, TURNOUT_WB_TRAILING_RH, TP( 7), TM( 7), 1);
    display_turnout(28, 10, TURNOUT_X_FACING_RH,    TP( 8), TM( 8), 1);
    display_turnout(25, 12, TURNOUT_EB_TRAILING_RH, TP( 9), TM( 9), 1);
    display_turnout(44, 10, TURNOUT_X_FACING_LH,    TP(11), TM(11), 1);
    display_turnout(47, 10, TURNOUT_WB_TRAILING_RH, TP(12), TM(12), 1);
    display_turnout(52, 10, TURNOUT_WB_FACING_RH,   TP(13), TM(13), 1);
    display_turnout(55, 10, TURNOUT_X_TRAILING_LH,  TP(14), TM(14), 1);
    display_turnout(60, 10, TURNOUT_WB_FACING_RH,   TP(15), TM(15), 1);
    display_turnout(65, 12, TURNOUT_EB_TRAILING_RH, TP(16), TM(16), 1);
    display_turnout(70, 10, TURNOUT_X_TRAILING_LH,  TP(28), TM(28), 1);
    display_turnout(10, 21, TURNOUT_X_FACING_LH,    TP(27), TM(27), 1);
    display_turnout(17, 23, TURNOUT_EB_FACING_RH,   TP( 0), TM( 0), 1);
    display_turnout(26, 21, TURNOUT_WB_TRAILING_RH, TP(32), TM(32), 1);
    display_turnout(34, 21, TURNOUT_WB_FACING_RH,   TP(31), TM(31), 1);
    display_turnout(40, 21, TURNOUT_WB_TRAILING_RH, TP( 0), TM( 0), 1);
    display_turnout(49, 21, TURNOUT_X_TRAILING_LH,  TP(25), TM(25), 1);
    display_turnout(52, 23, TURNOUT_EB_FACING_RH,   TP(26), TM(26), 0);
    display_turnout(64, 21, TURNOUT_X_FACING_LH,    TP(23), TM(23), 1);
    display_turnout(71, 21, TURNOUT_X_TRAILING_LH,  TP(24), TM(24), 1);
    display_turnout(74, 23, TURNOUT_EB_FACING_RH,   TP(29), TM(29), 0);
    //
    // Double slip #25 to #26
    // special case the switchstand...
    //
    if (TP(25)) {
	    switchstandcolor = RED;
	    turnoutcolor = RED;
    } else if (TM(25) || TM(26)) {
	    switchstandcolor = YELLOW;
	    turnoutcolor = RED;
    } else {
	    switchstandcolor = GREEN;
	    turnoutcolor = GREEN;
    }
    display_switchstand (50,24, SIGNAL_EAST, switchstandcolor);
}

