// @(#)rroperat.cc	1.3 99/08/06 SBHRS

/*
THIS PROGRAM IS THE PROGRAM FOR OPERATING UNDER
THE CONTROL OF A DISPATCHER DURING AN OPERATING
SESSION ON THE LAYOUT
*/


#include "rr.h"    // General purpose header file
#include "globals.h"
#include "port.h"  // Port comunications
#include "usic.h"  // USIC comunications
#include "kbdio.h"

// #include <unistd.h>

/************************************************************************/
/*                                                                      */
/* Input line options:                                                  */
/*  /A  Sets the assign/drop report mode to on.                         */
/*      The log file will be updated whenever an assign                 */
/*      or drop is made.                                                */
/*                                                                      */
/*  /T  Sets the turnout report mode to on.                             */
/*      The log file will be updated whenever a turnout                 */
/*      position is changed.                                            */
/*                                                                      */
/*  /D  Sets the detector report mode to on.                            */
/*      The log file will be updated whenever a detector                */
/*      changes state.                                                  */
/*                                                                      */
/************************************************************************/
// The folowing are command line variables passed upon startup

Options options;

BYTE     *OutBuf;   // Output Buffer
BYTE     *InBuf;    // Input Buffer
BYTE     *SaveBuf;  // Copy of Output Buffer

BYTE     CardType[17];                    // Card array for interface
int      Block[NumberBlocks + 1][BBParms];
char     cabnum[8][12];
int      logflag;           // Flag for error logging


static void
usage()
{
	cout << "Usage: rr [/v{isual} /a[ssign] /t{urnout} /d{etector} /w{indow}\n";
	cout << "\t\t/a Assign/Drop reporting\n";
	cout << "\t\t/t Turnout reporting\n";
	cout << "\t\t/d Detector reporting\n";
	cout << "\t\t/n All input via keyboard\n";
	cout << "\t\t/r auto-reassign dropped cabs\n";
	cout << "\t\t/q auto-quit when chubb I/O hangs\n";
	cout << "\t\t/1 use COM1:\n";
	cout << "\t\t/2 use COM2:\n";
	cout << "\t\t/3 use COM3:\n";
	cout << "\t\t/4 use COM4:\n";
	exit(0);
}



//******MAIN******MAIN******MAIN******MAIN******MAIN******MAIN******


int
main(int argc, char *argv[])
{
    char    c, ch;
    int     i;
    int     needusage = FALSE;
    int     defaultport = Port::COM1;
    char    scbuffer[4096];
    char    buf[80];
    USIC    *usic;


    setbuf(stdout, NULL);

    options.showswitchstands = FALSE;
    options.showblocksignals = FALSE;
    options.showunderground  = TRUE;
    options.showloopcounter  = TRUE;
    options.debug_screen     = FALSE;
    options.log_screen       = FALSE;
    options.cardio_screen    = FALSE;
    options.cardio_card      = -1;

    options.autoquit         = FALSE;	// -q
    options.cabreassign      = FALSE;	// -r
    options.assignment_debug = FALSE;	// -a
    options.turnout_debug    = FALSE;	// -t
    options.detector_debug   = FALSE;	// -d
    options.nochubb	     = FALSE;   // -n

    // options.nochubb	     = TRUE;

    // check command line input
    if (--argc >= 1) {
        for (i = 1; i <= argc; i++) {
            if ((argv[i][0] != '/') && (argv[i][0] != '-')) //check first character
                usage();
            c = argv[i][1];        // get 1st valid character
            if (c >= 'a') {
                c -= 32;       // make it upper case
            }
            logflag = FALSE;
            switch (c)  {        // get the options
		// Debugging
                case 'A' : options.assignment_debug = TRUE; logflag = TRUE; break;
                case 'T' : options.turnout_debug = TRUE;    logflag = TRUE; break;
                case 'D' : options.detector_debug = TRUE;   logflag = TRUE; break;
                case 'N' : options.nochubb = TRUE;          logflag = TRUE;
			   defaultport = Port::NONE;			    break;
                case 'Q' : options.autoquit = TRUE;    	    logflag = TRUE; break;


		// Features
                case 'R' : options.cabreassign = TRUE;      break;

		// Config
		case '1' : defaultport = Port::COM1; break;
		case '2' : defaultport = Port::COM2; break;
		case '3' : defaultport = Port::COM3; break;
		case '4' : defaultport = Port::COM4; break;
                default  : needusage     = TRUE;                break;
            }
        }
        if (needusage == TRUE) {
		usage();
        }
    }
    // Now, check for logging.  If required, log the fact we have started
    logmessage("------ Railroad log file ------");
    logmessage("The following Flags are set:");
    if (options.nochubb == TRUE)
	logmessage("Flag: options.nochubb");
    if (options.cabreassign == TRUE)
	logmessage("Flag: options.cabreassign");
    if (options.assignment_debug == TRUE)
	logmessage("Flag: options.assignment_debug");
    if (options.turnout_debug == TRUE)
	logmessage("Flag: options.turnout_debug");
    if (options.detector_debug == TRUE)
	logmessage("Flag: options.detector_debug");
    if (options.autoquit == TRUE)
	logmessage("Flag: options.autoquit");
    if (logflag != TRUE)
	logmessage("No debugging flags given");

    printf("Card Array\n");

    Cards *IOcard0 = new Cards(
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


    printf("Memory...\n");
    //  OutBuf = new BYTE * [1];
    //  InBuf = new BYTE * [1];
    //  SaveBuf = new BYTE * [1];

    OutBuf  = new BYTE[IOcard0->OutputBufferSize()];
    SaveBuf = new BYTE[IOcard0->OutputBufferSize()];
    InBuf   = new BYTE[IOcard0->InputBufferSize()];

    for (i=0; i < IOcard0->NumberOutputPorts(); i++) {
	OutBuf[i] = 0;
	SaveBuf[i] = 0;
    }
    for (i=0; i < IOcard0->NumberInputPorts(); i++) {
	InBuf[i]  = 0;
    }

    //Now, set up the USIC interface.  This sets up the port
    //  parameters and initializes the USIC uP with the card
    //  array info so it will know which cards are input and
    //  which are output cards.


    if (options.nochubb == TRUE) {
	    printf("Emulating USIC\n");
	    usic = new USIC(IOcard0, 0, Port::NONE, 0, options.autoquit);
    } else {
	    printf("Initializing USIC\n");
	    usic = new USIC(IOcard0, 0, defaultport, 9600, options.autoquit);
    }

    // Next, initialize the output buffer with the initial values
    //  i.e., set all lineside signals to red, all turnouts to
    //        normal, all throttles to zero, etc.

    printf("Output buffer\n");

    InitOutBuf(OutBuf);

    // Now, set up the Save Buffer for the first iteration
    printf("Get layout state from Chubb system\n");
    if (options.nochubb == FALSE) {
	    usic->Read(InBuf);
    }
    for (i = 0; i <= IOcard0->InputBufferSize(); i++) { //This relieves individual
	SaveBuf[i] = InBuf[i];                // routines from having to
    }			                      // perform this task themselves

    if (options.nochubb == TRUE) {
	    emulate_init(InBuf, OutBuf, SaveBuf);
    }

    printf("Checking for unlocked or diverging turnouts\n");
    checkmanuals();                 // Check Manual Reverse Switch positions
    checkcabdirections();           // Check cab directions

    SaveBuf[37] = SaveBuf[37] ^ 1;  // Fake program to think there is a change

			    // Set up the text screen.
    textmode(3);                    // Set for 16 color
    textcolor(BLACK);               // Text color
    textbackground(LIGHTGRAY);      // Background color
    clrscr();                       // Clear it
    update_screen(usic);
    clrdspmnu();
    dspmnu();

    for(;;) {
    	if (kbhit()) {
        	ch = getch();
        	if (ch >= 'a') {
			ch = ch - ('a' - 'A');
        	} 
		switch (ch) {
            	case 'A' : key_assign(usic);   break;
            	case 'D' : key_drop(usic);     break;
            	case 'T' : key_turnout(usic);  break;
            	case 'P' : key_panic(usic);    break;
            	case 'R' : key_route(usic);    break;
            	case 'O' : key_options(usic);  break;
		case '0' : OutBuf[1] = 0; break;
		case '1' : OutBuf[1] = 7; break;
		case 'C' :
            		if (options.cardio_screen == TRUE) {
				key_getcard(usic);
			}
			break;
		case '+' :
		case '-' :
            		if (options.cardio_screen == TRUE) {
				if (ch == '-' && options.cardio_card > 0) {
					options.cardio_card--;
				} else if (ch == '+' && options.cardio_card < 63) {
					options.cardio_card++;
				}
			}
			break;
		case 'E' :
            		if (options.nochubb == TRUE) {
				emulate();
				clrdspmnu();
				dspmnu();
			}
			break;
            	case 'Q' : 
			   gotoxy( 3, 3);
			   cprintf("%77s", "");
			   gotoxy(3, 3);
			   cprintf("Exiting...");
			   gotoxy(1,24);
			   exit(0);
            	default:   break;
        	}
    	}
    	RRLOOP(usic);
    }
}

int isValidBlock(int block)
{
        switch (block) {
		case 101: case 102: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110:
		case 111: case 112: case 113: case 114: case 115:
		case 116: case 117: case 118: case 119: case 120:
		case 121: case 122: case 123: case 124:
		case 201: case 202: case 203: case 204: case 205:
		case 206: case 207: case 208: case 209: case 210:
		case 211: case 212: case 213: case 214: case 215:
		case 216: case 217: case 218: case 219: case 220:
		case 221: case 222: case 223: case 224:
		case 401: case 402: case 403: case 404: case 405:
		case 406: case 407: case 408:
		                           // case 409: case 410:
		case 411: case 412: case 413: case 414: case 415:
		case 416: case 417: case 418:
			return 1;
		default:
			return 0;
	}
}

void key_assign(USIC *usic)
{
	int index, block = -1, cab;
	int xpos;
	char *roadnum;
	int error = 0;
	int detectedinblock = 0;
	char menu[80];

        clrdspmnu();
        gotoxy( 3, 2); cprintf("A)ssign: ");
	setmenu("   Block:___");
	xpos = 12;
	for(;;) {
		block   = getinputnumber(usic, xpos, 3, 3);
		if (block == -1) { error = 1; break; }
		if (isValidBlock(block)) break;
		sprintf(menu, "   Block %3d is invalid (try 101, 217...):___", block);
		setmenu(menu);
		xpos = 45;
	}
	if (!error) {
		sprintf(menu, "   Block:%3d Cab:_", block);
		xpos = 20;
		for(;;) {
			setmenu(menu);
			cab     = getinputnumber(usic, xpos, 3, 1);
			if (cab == -1) { error = 1; break; }
			if ((cab >= 1) && (cab <= 7)) break;
			sprintf(menu, "   Block:%3d Cab:%d is invalid (try 1-7):_", block, cab);
			setmenu("");
			xpos = 43;
		}
	}
	if (!error) {
		sprintf(menu, "   Block:%3d Cab:%d Road#:_________", block, cab);
		setmenu(menu);
		roadnum = getinputstring(usic, 28, 3, 9);
		if (strcmp(roadnum, "\026") == 0) {error = 1; }
	}

	if (!error) {
		setmenu("");
		gotoxy( 3, 3);

		if ( (block > 100) && (block < 199)) {
			index = block - 100;
		} else if ( (block > 200) && (block < 299)) {
			index = block - 200 + 24;
		} else if ( (block > 300) && (block < 399)) {
			index = block - 300 + 64;
		} else if ( (block > 400) && (block < 499)) {
			index = block - 400 + 48;
		}
		detectedinblock = isOccupied(InBuf, index);
		
		// detectedinblock = (InBuf[blockbuf[index][DETBUF]] >> blockbuf[index][DETS]) & 0x01;
		if (! detectedinblock) {
			cprintf("No train detected in block %3d", block);
			error = 1;
		} else if (blockbuf[index][CURCAB] != 0 ) {
			cprintf("Block %3d is in use", block);
			error = 1;
		}
	}
	if (error) {
		cprintf("  [Press any key]");
		do{
		    RRLOOP(usic);
		} while (!kbhit());
	} else {
		assignblock(OutBuf, index, cab);
		strcpy(&(cabnum[cab][0]), roadnum);
		delete[] roadnum;
		gotoxy( 3, 3);
		cprintf("Assigned");
	}
        clrdspmnu();	//clear dispatcher entry screen
        dspmnu(); 	// redraw dispatcher main menue
}


void key_drop(USIC *usic)
{
	int error = 0, block, logicalblock, cab, xpos;
	char menu[80];

        clrdspmnu();
        gotoxy( 3, 2); cprintf("D)rop: ");
	xpos = 12;
	for(;;) {
		setmenu("   Block:___");
		logicalblock   = getinputnumber(usic, xpos, 3, 3);
		if (logicalblock == -1) { error = 1; break; }
		if (isValidBlock(logicalblock)) break;
		sprintf(menu, "   Block %3d is invalid (try 101, 217...):___", logicalblock);
		setmenu(menu);
		xpos = 45;
	}
	if (! error) {
		setmenu("");
		gotoxy( 3, 3);

		if ( (logicalblock > 100) && (logicalblock < 199)) {
			block = logicalblock - 100;
		} else if ( (logicalblock > 200) && (logicalblock < 299)) {
			block = logicalblock - 200 + 24;
		} else if ( (logicalblock > 300) && (logicalblock < 399)) {
			block = logicalblock - 300 + 64;
		} else if ( (logicalblock > 400) && (logicalblock < 499)) {
			block = logicalblock - 400 + 48;
		}
		int detectedinblock = (InBuf[blockbuf[block][DETBUF]] >> blockbuf[block][DETS]) & 0x01;
		blockbuf[block][LASTCAB] = NOCAB;
		if (! detectedinblock) {
			cprintf("  Note: No train in block %3d.", logicalblock);
			error = 1;
		} else if (blockbuf[block][CURCAB] == 0 ) {
			cprintf("Note:  Block %3d was not in use.", logicalblock);
			error = 1;
		}
	}
	if (error) {
		cprintf("  [Press any key]");
		do{
		    RRLOOP(usic);
		} while (!kbhit());
	} else {
		int nextwest   = blockbuf[block][NEXTWEST];
		int nexteast   = blockbuf[block][NEXTEAST];
		int ab = NOBLOCK;
		int cab = blockbuf[block][CURCAB];
		if (cab == NOCAB) {
		    cab = blockbuf[block][LASTCAB];
		}

		cabnum[cab][0] = '\0';
		// blow away the lastcab info
	        if (cab == DCC || blockbuf[block][DIRECTION] == WESTBOUND) {
		    ab = nextwest;
		    if ((ab != NOBLOCK) &&
			(!isOccupied(InBuf, ab)) &&
			(blockbuf[ab][CURCAB] == cab)) {
			    dropblock(OutBuf, ab);
		    }
		} else if (cab == DCC || blockbuf[block][DIRECTION] == EASTBOUND) {
		    ab = nexteast;
		    if ((ab != NOBLOCK) &&
			(!isOccupied(InBuf, ab)) &&
			(blockbuf[ab][CURCAB] == cab)) {
			    dropblock(OutBuf, ab);
		    }
		}
		dropblock(OutBuf, block);


		gotoxy( 3, 3);
		cprintf("   Block %3d Dropped.", logicalblock);
	}

        clrdspmnu();	//clear dispatcher entry screen
        dspmnu(); 	// redraw dispatcher main menue
}

static void normalize_turnout(BYTE *OutBuf, int turn)
{
	if (turnout[turn][LOCK] == UNLOCKED) {
		return;
	}
	OutBuf[turnout[turn][CONTROL_PORT]] &=
		(0xff ^ (0x01 << turnout[turn][CONTROL_BITS]));
	if (options.nochubb == TRUE) {
		em_normalize(turn);
	}
}
static void reverse_turnout(BYTE *OutBuf, int turn)
{
	if (turnout[turn][LOCK] == UNLOCKED) {
		return;
	}
	OutBuf[turnout[turn][CONTROL_PORT]] |=
		(0x01 << turnout[turn][CONTROL_BITS]);
	if (options.nochubb == TRUE) {
		turnout[turn][POSITION] = NORMAL;
		em_reverse(turn);
	}
}
static void toggle_turnout(BYTE *OutBuf, int turn)
{
	if (turnout[turn][LOCK] == UNLOCKED) {
		return;
	}
	OutBuf[turnout[turn][CONTROL_PORT]] ^=
		(0x01 << turnout[turn][CONTROL_BITS]);
	if (options.nochubb == TRUE) {
		if (turnout[turn][POSITION] == NORMAL) {
		    em_reverse(turn);
		} else {
		    em_normalize(turn);
		}
	}
}

void key_turnout(USIC *usic)
{
	int turn;
	int error = 0;
	char *dir;
	char menu[80];

        clrdspmnu();
        gotoxy( 3, 2); cprintf("T)urnout: ");
        sprintf(menu, "   Turnout number:__");
	int xpos = 21;
	for(;;) {
		setmenu(menu);
		turn = getinputnumber(usic, xpos, 3, 2);
		if (turn < 0) {
			error = 1;
			break;
		}
		if (turn > NumberTurnouts) {
        		sprintf(menu, "   Turnout number %2d is invalid (Try 01-%02d):__ ",
				turn, NumberTurnouts);
			xpos = 46;
			setmenu(menu);
		} else if (turnout[turn][LOCK] == UNLOCKED) {
			sprintf(menu, "   Turnout number %02d is in use on a remote panel. ", turn);
			setmenu(menu);
			error++;
			break;
		} else {
			break;
		}
	}
	if (!error) {
		sprintf(menu, "   Turnout number %2d N)ormal,D)iverging,T)oggle:_", turn);
		setmenu(menu);
		gotoxy(53,3); cprintf("(currently ");

		if(turnout[turn][POSITION] == REVERSE) {
			textcolor(RED);
			cprintf("D");		// if computer reverse
		} else {
			textcolor(GREEN);
			cprintf("N");
		}
		textcolor(BLACK);
		cprintf(")");

		for(;;) {
			int ok = 1;
			dir = getinputstring(usic, 50, 3, 1);
			if (*dir == '\0') {
				error++;
			} else switch (*dir) {
				case 'n': case 'N': normalize_turnout(OutBuf, turn); break;
				case 'd': case 'D': reverse_turnout(OutBuf, turn);   break;
				case 't': case 'T': toggle_turnout(OutBuf, turn);    break;
				default: ok = 0; break;
			}
			if (ok) break;
		}
	}
	if (error) {
		cprintf("  [Press any key]");
		do{
		    RRLOOP(usic);
		} while (!kbhit());
	}
        clrdspmnu();	//clear dispatcher entry screen
        dspmnu(); 	// redraw dispatcher main menue
}

void key_route(USIC *usic)
{
	int block;

        clrdspmnu();
        gotoxy( 3, 2); cprintf("R)oute: ");
        setmenu("   Route:___");
	block   = getinputnumber(usic, 12, 3, 2);

	switch (block) {
		case 1: // track 1 Freight siding at Durango
			toggle_turnout(OutBuf, 1);
			toggle_turnout(OutBuf, 9);
			break;
		case 2: // track 2 Passenger siding at Durango
			toggle_turnout(OutBuf, 2);
			normalize_turnout(OutBuf, 3);
			normalize_turnout(OutBuf, 4);
			normalize_turnout(OutBuf, 6);
			normalize_turnout(OutBuf, 7);
			toggle_turnout(OutBuf, 8);
			break;
		case 22: // track 2 Passenger siding at Sequoia
			toggle_turnout(OutBuf, 31);
			toggle_turnout(OutBuf, 32);
			break;
		case 26: // out of staging
			toggle_turnout(OutBuf, 25);
			toggle_turnout(OutBuf, 26);
			break;
		// West staging
		case 4:
			normalize_turnout(OutBuf, 45);
			normalize_turnout(OutBuf, 46);

			normalize_turnout(OutBuf, 41);
			normalize_turnout(OutBuf, 42);
			break;
		case 11:
			reverse_turnout(OutBuf, 45);

			reverse_turnout(OutBuf, 41);
			normalize_turnout(OutBuf, 42);
			break;
		case 12:
			normalize_turnout(OutBuf, 45);
			reverse_turnout(OutBuf, 46);
			normalize_turnout(OutBuf, 47);

			normalize_turnout(OutBuf, 43);
			reverse_turnout(OutBuf, 42);
			break;
		case 13:
			normalize_turnout(OutBuf, 45);
			reverse_turnout(OutBuf, 46);
			reverse_turnout(OutBuf, 47);
			normalize_turnout(OutBuf, 48);

			normalize_turnout(OutBuf, 44);
			reverse_turnout(OutBuf, 43);
			reverse_turnout(OutBuf, 42);
			break;
		case 14:
			normalize_turnout(OutBuf, 45);
			reverse_turnout(OutBuf, 46);
			reverse_turnout(OutBuf, 47);
			reverse_turnout(OutBuf, 48);

			reverse_turnout(OutBuf, 44);
			reverse_turnout(OutBuf, 43);
			reverse_turnout(OutBuf, 42);
			break;
		// East Staging
		case 18:
			// normalize_turnout(OutBuf, 37);
			normalize_turnout(OutBuf, 33);
			break;
		case 17:
			// reverse_turnout(OutBuf, 37);
			// normalize_turnout(OutBuf, 38);

			normalize_turnout(OutBuf, 34);
			reverse_turnout(OutBuf, 33);
			break;
		case 16:
			// reverse_turnout(OutBuf, 37);
			// reverse_turnout(OutBuf, 38);
			// normalize_turnout(OutBuf, 39);

			normalize_turnout(OutBuf, 35);
			reverse_turnout(OutBuf, 34);
			reverse_turnout(OutBuf, 33);
			break;
		case 15:
			// reverse_turnout(OutBuf, 37);
			// reverse_turnout(OutBuf, 38);
			// reverse_turnout(OutBuf, 39);
			// normalize_turnout(OutBuf, 40);

			normalize_turnout(OutBuf, 36);
			reverse_turnout(OutBuf, 35);
			reverse_turnout(OutBuf, 34);
			reverse_turnout(OutBuf, 33);
			break;
		case  6:
			// reverse_turnout(OutBuf, 37);
			// reverse_turnout(OutBuf, 38);
			// reverse_turnout(OutBuf, 39);
			// reverse_turnout(OutBuf, 40);

			reverse_turnout(OutBuf, 36);
			reverse_turnout(OutBuf, 35);
			reverse_turnout(OutBuf, 34);
			reverse_turnout(OutBuf, 33);
			break;
		default:
			cprintf(" Invalid Route.  Press any key");
			do{
			    RRLOOP(usic);
			} while (!kbhit());
	}

        clrdspmnu();	//clear dispatcher entry screen
        dspmnu(); 	// redraw dispatcher main menue
}


void key_getcard(USIC *usic)
{
	int card;

        clrdspmnu();
        gotoxy( 3, 2); cprintf("C)ard Debugger: ");
        setmenu("   Card to watch:__  (00-63) [<ESC> = all]");
	card   = getinputnumber(usic, 20, 3, 2);
	options.cardio_card = card;

        clrdspmnu();	//clear dispatcher entry screen
        dspmnu(); 	// redraw dispatcher main menue
}


void key_options(USIC *usic)
{
	char *opt;
	char m[80];

        clrdspmnu();
        gotoxy( 3, 2); cprintf("O)ptions: ");
	sprintf(m, "   B)lks S)witch U)ndergnd N)umber T)rack C)hubb L)og W)ire R)eassign D)ebug:");
	setmenu(m);
	opt   = getinputstring(usic, 3 + strlen(m), 3, 1);

        setmenu("");
	gotoxy( 3, 3);
        if (*opt == 'S' || *opt == 's') {
		options.showswitchstands = ! options.showswitchstands;
		cprintf("    Switchstands will %sbe shown",
				options.showswitchstands?"" : "not ");
	} else if (*opt == 'N' || *opt == 'n') {
		options.showloopcounter = ! options.showloopcounter;
		cprintf("    Loop counter will %sbe shown",
				options.showloopcounter?"" : "not ");
	} else if (*opt == 'B' || *opt == 'b') {
		options.showblocksignals = ! options.showblocksignals;
		cprintf("    Block Signals will %sbe shown",
				options.showblocksignals?"" : "not ");
	} else if (*opt == 'U' || *opt == 'u') {
		options.showunderground = ! options.showunderground;
		cprintf("    Underground Staging tracks will %sbe shown",
				options.showunderground?"" : "not ");
	} else if (*opt == 'E' || *opt == 'e') {
		options.nochubb = ! options.nochubb;
		cprintf("    Emulation %s",
				options.nochubb ? "On" : "Off");
	} else if (*opt == 'T' || *opt == 't') {
		options.debug_screen = FALSE;
		options.cardio_screen = FALSE;
		options.log_screen = FALSE;
		cprintf("    Track Diagram will be shown");
	} else if (*opt == 'C' || *opt == 'c') {
		options.debug_screen = TRUE;
		options.cardio_screen = FALSE;
		options.log_screen = FALSE;
		cprintf("    Chubb Screen will be shown");
	} else if (*opt == 'W' || *opt == 'w') {
		options.cardio_screen = TRUE;
		options.debug_screen = FALSE;
		options.log_screen = FALSE;
		cprintf("    Wiring Screen will be shown");
	} else if (*opt == 'L' || *opt == 'l') {
		options.log_screen = TRUE;
		options.cardio_screen = FALSE;
		options.debug_screen = FALSE;
		cprintf("    Log Messages will be shown");
	} else if (*opt == 'R' || *opt == 'r') {
		cprintf("    Auto reassign dropped cabs %s",
				options.cabreassign ? "On" : "Off");
		if (options.cabreassign == TRUE) {
		    options.cabreassign = FALSE;
		    logmessage("Disabling auto-reassign");
		} else {
		    options.cabreassign = TRUE;
		    logmessage("Enabling auto-reassign");
		}
	} else if (*opt == 'D' || *opt == 'd') {
	    char m[80];
	    clrdspmnu();
	    gotoxy( 3, 2); cprintf("O)ptions D)ebug: ");
	    sprintf(m, "   A)ssignments [%s] D)etectors [%s] T)urnouts [%s]:",
		    (options.assignment_debug == FALSE) ? "Off" : "On",
		    (options.detector_debug == FALSE) ? "Off" : "On",
		    (options.turnout_debug == FALSE) ? "Off" : "On");
	    setmenu(m);
	    opt   = getinputstring(usic, 3 + strlen(m), 3, 1);
	    setmenu("");
	    gotoxy( 3, 3);
	    if (*opt == 'A' || *opt == 'a') {
		if (options.assignment_debug == TRUE) {
		    options.assignment_debug = FALSE;
		    logmessage("assignment debugging OFF");
		} else {
		    options.assignment_debug = TRUE;
		    logmessage("assignment debugging ON");
		}
	    } else if (*opt == 'T' || *opt == 't') {
		if (options.turnout_debug == TRUE) {
		    options.turnout_debug = FALSE;
		    logmessage("turnout debugging OFF");
		} else {
		    options.turnout_debug = TRUE;
		    logmessage("turnout debugging ON");
		}
	    } else if (*opt == 'D' || *opt == 'd') {
		if (options.detector_debug == TRUE) {
		    options.detector_debug = FALSE;
		    logmessage("detector debugging OFF");
		} else {
		    options.detector_debug = TRUE;
		    logmessage("detector debugging ON");
		}
	    }
	}
        clrdspmnu();	//clear dispatcher entry screen
        dspmnu(); 	// redraw dispatcher main menue
}


void RRLOOP(USIC *usic)
{
	int i;
	if (options.nochubb == FALSE) {
		usic->Read(InBuf);
	} else {
		sleep(1);
	}
	panic(usic);            // Check for Panic Button!
	checkcabdirections();   // Check cab directions
	checkturnouts();        // Check turnouts
	CheckSubBlock();        // Check sub-block occupancy and forces main block bits to be set
	// throttlebutton();       // Check throttle push buttons (unused)
	assigndrop(InBuf, OutBuf, SaveBuf);           // Check Assign/Drop panels
	autoad(InBuf, OutBuf, SaveBuf);               // Assign/drop new blocks
	do_signals();           // Adjust lineside signals
	update_screen(usic);

	for (i = 0; i <= NumberInputPorts; i++) {
		SaveBuf[i] = InBuf[i];
	}
	if (options.nochubb == FALSE) {
	    	usic->Write(OutBuf);
	}
}

