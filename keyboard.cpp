// @(#)keyboard.cc	1.2 99/08/06 SBHRS

// keyboardin

#include    "rr.h"
#include    "globals.h"
#include <ctype.h>

static char str[40];

static BYTE zero(BYTE buffer, BYTE mask, int shift);
static void emulate_panel(void);
static void emulate_turnout(void);
static void emulate_detector(void);
static void emulate_cab(void);
static void emulate_cab_direction(int cab);
static void emulate_cab_button(int cab);

static void em_assign(int inpanel, int mainline, int cab, int block);
static void em_drop(int inpanel);
static void show_input_buffer(void); //
static void show_output_buffer(void);
static void fcab(int in);
static void rcab(int in);
static void tcab(int in);


//keyboardin accepts input from the keyboard and stuffs it into
//  the input buffer 

BYTE     *localOutBuf;   // Output Buffer
BYTE     *localInBuf;    // Input Buffer
BYTE     *localSaveBuf;  // Copy of Output Buffer

void emulate_init(BYTE *InBuf, BYTE *OutBuf, BYTE *SaveBuf)
{
	int turn;

        localOutBuf  = OutBuf;
        localInBuf   = InBuf;
        localSaveBuf = SaveBuf;

	// Need to pretend that the layout is indicating that the turnouts
	// are not locally unlocked...
        for (turn = 1; turn <= NumberTurnouts; turn++) {
	        em_lock(turn);
        }
}

void emulate()
{
	char subcmd;
	clrdspmnu();
        gotoxy( 3, 2); cprintf("E)mulate chubb I/O from keyboard:");
        gotoxy( 3, 3); cprintf("    P)anel T)urnout D)etector C)ab I)nbuf O)utbuf:_");
	gotoxy(53, 3);
	subcmd = getch();


        switch (subcmd){
        case 'p': case 'P': emulate_panel();	break;
	case 't': case 'T': emulate_turnout();	break;
	case 'd': case 'D': emulate_detector();	break;
	case 'c': case 'C': emulate_cab();	break;
	case 'i': case 'I': show_input_buffer();	break;
	case 'o': case 'O': show_output_buffer();	break;
	}
}

//Keyboard entry of assign/drop data
static void
emulate_panel(void)
{
    int     panel = -1, mainline = -1, cab = -1, block = -1;
    char    ad, c;

    // Get input and do error checking.
	clrdspmnu();
        gotoxy( 3, 2); cprintf("E)mulate P)anel:");
        gotoxy( 3, 3); cprintf("    Panel:_ Main:_ Cab:_ Block:__ Assign/Drop:_");

	while (panel == -1) {
    		gotoxy(13, 3); 
    		c = getch();
		if (c == (char)27) return;
    		if ((c <= '0') || (c > '6')) {
        		gotoxy(52, 3); cprintf("<Panel must be 1 to 6>");
    		} else {
			cprintf("%c", c);
		        gotoxy(52, 3); cprintf("                      ");
		        panel = c - '0';
		}
	}

    // get mainline number
	while (mainline == -1) {
    		gotoxy(20, 3); 
    		c = getch();
		if (c == (char)27) return;
    		if ((c != '1') &&
		    (c != '2') &&
		    (c != '3') &&
		    (c != '4')) {
        		gotoxy(52, 3); cprintf("<Main must be 1, 2, 3 or 4>");
    		} else {
			cprintf("%c", c);
        		gotoxy(52, 3); cprintf("                           ");
		        mainline = c - '0';
		}
	}



    // get cab number
	while (cab == -1) {
    		gotoxy(26, 3); 
    		c = getch();
		if (c == (char)27) return;
    		if ((c < '1') || (c > '7')) {
        		gotoxy(52, 3); cprintf("<Cab must be 1 to 7>");
    		} else {
			cprintf("%c", c);
		        gotoxy(52, 3); cprintf("                    ");
		        cab = c - '0';
		}
	}

    // Get block number
        int lc = 0;
	int num = 0;
	while (lc < 2) {
    		gotoxy(34, 3); 
    		c = getch();
		if (c == (char)27) return;
    		if ((c < '0') || (c > '9')) {
        		gotoxy(52, 3); cprintf("<Block is 00 to 99>");
			num = 0;
			lc = 0;
    		} else {
		        num = (num * 10) + (c - '0');
			cprintf("%2d", num);
		        gotoxy(52, 3); cprintf("                   ");
			lc++;
		}
	}
	block = num;
 
	ad = ' ';
	while (!((ad == 'A') || (ad == 'D') )) {
		gotoxy(49, 3); 
		c = getch();
	        if (islower(c)) {
			c = toupper(c);       // make it upper case
		}

    		if (!((c == 'A') || (c == 'D') )) {
        		gotoxy(52, 3); cprintf("<Must be A or D>");
        	} else {
			cprintf("%c", c);
        		gotoxy(52, 3); cprintf("                ");
			ad = c;
		}
        }
        switch (ad) {
        case 'A' :
            em_assign(panel, mainline, cab, block);
            break;
        case 'D' :
            em_drop(panel);
            break;
        default  :
	    break;
    }
}

//Keyboard entry of turnout data
static void
emulate_turnout(void)
{
        int turn;
        char state, c;

	clrdspmnu();
        gotoxy( 3, 2); cprintf("E)mulate T)urnout");
        gotoxy( 3, 3); cprintf("    Turnout:__ Normal/Diverging/Toggle/Unlock/Lock/:_ ");

        int lc = 0;
	int num = 0;
	while (lc < 2) {
    		gotoxy(15, 3); 
    		c = getch();
		if (c == (char)27) return;
    		if ((c < '0') || (c > '9')) {
        		gotoxy(52, 3); cprintf("<Turnout is 00 to %d>", NumberTurnouts);
			num = 0;
			lc = 0;
    		} else {
		        num = (num * 10) + (c - '0');
			cprintf("%2d", num);
			if (num > NumberTurnouts) {
				gotoxy(52, 3); cprintf("<Turnout is 00 to %d>", NumberTurnouts);
				num = 0;
				lc = 0;
			} else {
				gotoxy(52, 3); cprintf("                      ");
				lc++;
			}
		}
	}
	turn = num;
 
	state = ' ';
	while (!((state == 'N') ||
		 (state == 'D') ||
		 (state == 'T') ||
		 (state == 'U') ||
		 (state == 'L') )) {
		gotoxy(15 + 40, 3);
		c = getch();
	        if (islower(c)) {
			c = toupper(c);       // make it upper case
		}

    		if (!((c == 'N') ||
		      (c == 'D') ||
		      (c == 'T') ||
		      (c == 'U') ||
		      (c == 'L') )) {
        		gotoxy(52, 3); cprintf("<Must be N,R,T,U or L>");
        	} else {
			cprintf("%c", c);
        		gotoxy(52, 3); cprintf("                      ");
			state = c;
		}
        }
        switch (state) {
        case 'N' : em_normalize(turn);	break;
        case 'D' : em_reverse(turn);	break;
        case 'T' : em_toggle(turn);	break;
        case 'U' : em_unlock(turn);	break;
        case 'L' : em_lock(turn);	break;
        default : break;
    }
}

//Keyboard entry of detector data
static void
emulate_detector(void)
{
        int detector;
        char oct, c;

	clrdspmnu();
        gotoxy( 3, 2); cprintf("E)mulate D)etector");
        gotoxy( 3, 3); cprintf("    Detector:__ Occupied/Clear/Toggle:_ ");

        int lc = 0;
	int num = 0;
	while (lc < 2) {
    		gotoxy(16, 3); 
    		c = getch();
		if (c == (char)27) return;
    		if ((c < '0') || (c > '9')) {
        		gotoxy(52, 3); cprintf("<Detector is 00 to 72>");
			num = 0;
			lc = 0;
    		} else {
		        num = (num * 10) + (c - '0');
			cprintf("%2d", num);
			if (num > 72) {
				gotoxy(52, 3); cprintf("<Detector is 00 to 72>");
				num = 0;
				lc = 0;
			} else {
				gotoxy(52, 3); cprintf("                      ");
				lc++;
			}
		}
	}
	detector = num;
 
	oct = ' ';
	while (!((oct == 'O') || (oct == 'C') || (oct == 'T'))) {
		gotoxy(41, 3); 
		c = getch();
		if (c == (char)27) return;
	        if (islower(c)) {
			c = toupper(c);       // make it upper case
		}

    		if (!((c == 'O') || (c == 'C') || (c == 'T') )) {
        		gotoxy(52, 3); cprintf("<Must be O, C or T>");
        	} else {
			cprintf("%c", c);
        		gotoxy(52, 3); cprintf("                   ");
			oct = c;
		}
        }

        switch (oct) {
        case 'O' :
            localInBuf[blockbuf[detector][DETBUF]] |= 1 << blockbuf[detector][DETS];
            break;
        case 'C' :
            localInBuf[blockbuf[detector][DETBUF]] &= ~(1 << blockbuf[detector][DETS]);
            break;
        case 'T' :
            localInBuf[blockbuf[detector][DETBUF]] ^= 1 << blockbuf[detector][DETS];
            break;
        default :
            break;
    }
}

static void
emulate_cab(void)
{
	int num = 0;
        char c;
        int cab;
	char funct;

	clrdspmnu();
        gotoxy( 3, 2); cprintf("E)mulate C)ab");
        gotoxy( 3, 3); cprintf("    Cab:_ Speed/Dir/Button:_");

	while (num == 0) {
    		gotoxy(11, 3); 
    		c = getch();
		if (c == (char)27) return;
    		if ((c < '1') || (c > '7')) {
        		gotoxy(52, 3); cprintf("<cab is 1 to 7>");
			num = 0;
    		} else {
		        num = (c - '0');
			cprintf("%1d", num);
		}
	}
	cab = num;
 
	c = ' ';
	while (!((c == 'S') || (c == 'D') || (c == 'B'))) {
		gotoxy(30, 3); 
		c = getch();
		if (c == (char)27) return;
	        if (islower(c)) {
			c = toupper(c);       // make it upper case
		}

    		if (!((c == 'S') || (c == 'D') || (c == 'B') )) {
        		gotoxy(52, 3); cprintf("<Must be S, D or B>");
        	} else {
			cprintf("%c", c);
        		gotoxy(52, 3); cprintf("                   ");
		}
        }
	funct = c;

        switch (funct) {
        case 'S' :
        	if (throttle[cab][T_EM_SPEED] == 1) {
		    throttle[cab][T_EM_SPEED] = 0;
		    cprintf(" ON");
		} else {
		    throttle[cab][T_EM_SPEED] = 1;
		    cprintf(" OFF");
		}
            break;
        case 'D' :
            emulate_cab_direction(cab);
            break;
        case 'B' :
            emulate_cab_button(cab);
            break;
        default :
            break;
    }
}

//Keyboard entry of cab direction data
static void
emulate_cab_direction(int cab)
{
    char    c = ' ';

    clrdspmnu();
    gotoxy( 3, 2); cprintf("E)mulate C)ab D)irection");
    gotoxy( 3, 3); cprintf("    Cab:%d Dir E)ast, W)est or T)oggle:_", cab);

    while (c == ' ') {
	    gotoxy(41, 3); 
	    c = getch();
	    if (c == (char)27) return;
	    if (islower(c)) {
		c = toupper(c);       // make it upper case
            }

	    if ((c != 'W') && (c != 'E') && (c != 'T')) {
		    gotoxy(52, 3); cprintf("<Must be E, W or T>");
		    c = ' ';
	    }
    }

    switch (c) {
        case 'W' : fcab(cab); cprintf(" WEST");   break;
        case 'E' : rcab(cab); cprintf(" EAST");   break;
        case 'T' : tcab(cab); cprintf(" Toggle"); break;
        default :
            break;
    }
}

// push the throttle pushbutton!
static void
emulate_cab_button(int cab)
{
    BYTE    tmp;
    char c = ' ';

    clrdspmnu();
    gotoxy( 3, 2); cprintf("E)mulate C)ab B)utton");
    gotoxy( 3, 3); cprintf("    Cab:%d Button P)ush, R)elease or T)oggle:_", cab);

    while (c == ' ') {
	    gotoxy(44, 3); 
	    c = getch();
	    if (c == (char)27) return;
	    if (islower(c)) {
		c = toupper(c);       // make it upper case
            }

	    if ((c != 'P') && (c != 'R') && (c != 'T')) {
		    gotoxy(52, 3); cprintf("<Must be P, R or T>");
		    c = ' ';
	    }
    }

    tmp = 0x01;
    tmp = tmp << throttle[cab][3];

    switch (c) {
        case 'P' :
        	localInBuf[throttle[cab][2]] |= tmp;
		cprintf(" Push");   break;
        case 'R' :
		localInBuf[throttle[cab][2]] &= ~tmp;
	        cprintf(" Release");   break;
        case 'T' :
		if(localInBuf[throttle[cab][2]] & tmp) {
		    localInBuf[throttle[cab][2]] &= ~tmp;
		} else {
		    localInBuf[throttle[cab][2]] |= tmp;
		}
	        cprintf(" Toggle"); break;
        default :
            break;
    }
}


static void
em_assign(int inpanel, int mainline, int cab, int block)
{
    int     block10, block1, cab1, main1;

    // Turn on assign button
    localInBuf[1] = localInBuf[1] | panel[inpanel][0];

    // First, set mainline bits in input buffer.
    //  to do this, we shift the input to the correct bit position.
    //  then we OR it with the input buffer value.
    //  We'll us this procedure for each parameter

    main1 = mainline << panel[inpanel][2];

    //  mask out old value so we don't get eroneous values
    localInBuf[panel[inpanel][1]] = zero(localInBuf[panel[inpanel][1]], 0x03, panel[inpanel][2]);
    localInBuf[panel[inpanel][1]] = localInBuf[panel[inpanel][1]] | (BYTE)main1;

    // Next, extract the block 10's value and insert it as above.
    block10 = (int)(block / 10);
    block10 = block10 << panel[inpanel][4];

    //  mask out old value so we don't get eroneous values
    localInBuf[panel[inpanel][3]] = zero(localInBuf[panel[inpanel][3]], 0x03, panel[inpanel][4]);
    localInBuf[panel[inpanel][3]] = localInBuf[panel[inpanel][3]] | (BYTE)block10;

    // Now, do the block 1's.
    block1 = (int)(block % 10);
    block1 = block1 << panel[inpanel][6];

    //  mask out old value so we don't get eroneous values
    localInBuf[panel[inpanel][5]] = zero(localInBuf[panel[inpanel][5]], 0x0f, panel[inpanel][6]);
    localInBuf[panel[inpanel][5]] = localInBuf[panel[inpanel][5]] | (BYTE)block1;

    // Last, do the cab number
    cab1 = cab << panel[inpanel][8];

    //  mask out old value so we don't get eroneous values
    localInBuf[panel[inpanel][7]] = zero(localInBuf[panel[inpanel][7]], 0x07, panel[inpanel][8]);
    localInBuf[panel[inpanel][7]] = localInBuf[panel[inpanel][7]] | (BYTE)cab1;

    // Call assigndrop so that it sees the assign button pushed.
    assigndrop(localInBuf, localOutBuf, localSaveBuf);

    // Finally, 'un'push the assign button
    localInBuf[1] = localInBuf[1] & ~(panel[inpanel][0]);
}


static void
em_drop(int inpanel)
{
    int     block10, block1, cab1, main1;

    //Turn on drop button
    localInBuf[2] = localInBuf[2] | panel[inpanel][0];

    // First, set mainline bits in input buffer.
    //  to do this, we shift the input to the correct bit position.
    //  then we complement it and AND it with the input buffer value.
    //  We'll us this procedure for each parameter
    main1 = 0x03 << panel[inpanel][2];
    main1 = ~main1;
    localInBuf[panel[inpanel][1]] = localInBuf[panel[inpanel][1]] & (BYTE)main1;

    // Next, extract the block 10's value and insert it as above.
    localInBuf[1] = localInBuf[1] | panel[inpanel][0];
    block10 = 0x03 << panel[inpanel][4];
    block10 = ~block10;
    localInBuf[panel[inpanel][3]] = localInBuf[panel[inpanel][3]] & (BYTE)block10;

    // Now, do the block 1's.
    block1 = 0x0F << panel[inpanel][6];
    block1 = ~block1;
    localInBuf[panel[inpanel][5]] = localInBuf[panel[inpanel][5]] & (BYTE)block1;

    // Last, do the cab number
    cab1 = 0x07 << panel[inpanel][8];
    cab1 = ~cab1;
    localInBuf[panel[inpanel][7]] = localInBuf[panel[inpanel][7]] & (BYTE)cab1;

    // Call assigndrop so that it sees the assign button pushed.
    assigndrop(localInBuf, localOutBuf, localSaveBuf);

    // Finally, 'un'push the drop button
    localInBuf[2] = localInBuf[2] & ~(panel[inpanel][0]);
}

void show_input_buffer(void)
{
}

static void
show_output_buffer(void)
{
}

//em_normalize sets the turnout to normal
void
em_normalize(int in)
{
    char    tbuf[4096];
    BYTE    mask;

    //first, set the output control signal
    mask = 0x01 << turnout[in][CONTROL_BITS];
    mask = ~mask;
    localOutBuf[turnout[in][CONTROL_PORT]] &= (BYTE)mask;

    //now set the input signal
    mask = 0x01 << turnout[in][POSITION_BITS];
    mask = ~mask;
    localInBuf[turnout[in][POSITION_PORT]] &= (BYTE)mask;
}
void
em_reverse(int in)
{
    char    tbuf[4096];
    BYTE    mask;

    //first, set the output control signal
    mask = 0x01 << turnout[in][CONTROL_BITS];
    localOutBuf[turnout[in][CONTROL_PORT]] |= (BYTE)mask;

    //now set the input signal
    mask = 0x01 << turnout[in][POSITION_BITS];
    localInBuf[turnout[in][POSITION_PORT]] |= (BYTE)mask;
}

//em_lock sets the turnout to ctc
//em_unlock sets the turnout to manual

void
em_lock(int in)
{
    char    tbuf[4096];
    BYTE    mask;

    // clear the manual switch indication (0 == locked)
    mask = 0x01 << turnout[in][MANUAL_BITS];
    localInBuf[turnout[in][MANUAL_PORT]] |= (BYTE)mask;
}


void
em_unlock(int in)
{
    char    tbuf[4096];
    BYTE    mask;

    // set the manual switch indication (1 = unlocked)
    mask = 0x01 << turnout[in][MANUAL_BITS];
    mask = ~mask;
    localInBuf[turnout[in][MANUAL_PORT]] &= (BYTE)mask;
}

void
em_toggle(int in)
{
    char    tbuf[4096];
    BYTE mask;

    mask = 0x01 << turnout[in][POSITION_BITS];
    if (localInBuf[turnout[in][POSITION_PORT]] & (BYTE)mask) {
	// normalize
	em_normalize(in);
    } else {
	// reverse
	em_reverse(in);
    }
}

// fcab sets the input bit for the selected cab to foward
static void
fcab(int in)
{
    BYTE tmp;

    tmp = 0x01;
    tmp = tmp << throttle[in][1];
    localInBuf[throttle[in][0]] &= ~tmp;
}

// rcab clears the input bit for the selected cab to reverse
static void
rcab(int in)
{
    BYTE tmp;

    tmp = 0x01;
    tmp = tmp << throttle[in][1];
    localInBuf[throttle[in][0]] |= tmp;

}

// tcab toggles the input bit for the selected cab
static void
tcab(int in)
{
    BYTE tmp;

    tmp = 0x01;
    tmp = tmp << throttle[in][1];
    if(localInBuf[throttle[in][0]] & tmp) {
        fcab(in);
    } else {
        rcab(in);
    }
}

static BYTE
zero(BYTE buffer, BYTE mask, int shift)
{
    BYTE    shiftedmask;
    shiftedmask = ~(mask << shift);
    return(buffer & shiftedmask);
}


