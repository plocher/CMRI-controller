// @(#)globals.h	1.3 99/08/27 SBHRS

// Global variables

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

// #define VERSION	"20011029jp"
#define VERSION	"20020806jp"

const int   NumberInputCards    = 18;
const int   NumberOutputCards   = 46;
const int   NumberCardSets      = 16;
const int   NumberOutputPorts   = NumberOutputCards * 3;
const int   NumberInputPorts    = NumberInputCards  * 3;
const int   NumberBlocks        = 72;
const int   NumberSubs          = 30;
const int   NumberTurnouts      = 48;
const int   NumberCabs		=  7;
const int   numbersigs          =  2;


const int   NOBLOCK         = 99;   // No possible NEXTBLOCK
const int   CLEAR           = 0;    // Block status
const int   OCCUPIED        = 1;
const int   NOCAB           = 0;    // Cab assignment status
const int   NODIRECTION     = 0;    // Cab directions
const int   WESTBOUND       = 1;
const int   EASTBOUND       = 2;

const int   PRESSED         = 1;    // Assign/drop button pressed
const int   PUSHED          = 1;

//Turnouts
const int   LOCKED	    = 0;
const int   UNLOCKED	    = 1;    // Under manual control
const int   NORMAL          = 0;    // Turnout positions
const int   REVERSE         = 1;


// Pointers into blockbuf array
const int   BBParms         = 15;   // # of bockbuf parameters
const int   OBUF            = 0;    // OutBuf pointer
const int   OBUFS           = 1;    // Number of bits to shift
const int   NEXTWEST        = 2;    // Next West block
const int   NEXTEAST        = 3;    // Next East block
const int   PHYSICAL        = 4;    // Translation to physical block
const int   DETBUF          = 5;    // Detector buffer number
const int   DETS            = 6;    // Detector buffer offset
const int   WSIGBASE        = 7;    // West signal buffer base
const int   WSIGOFF         = 8;    // West signal buffer offset
const int   CURCAB          = 9;    // Currently assigned cab
const int   DIRECTION       =10;    // Current direction (1=W, 2=E)
const int   ESIGBASE        =11;    // East signal buffer base
const int   ESIGOFF         =12;    // East signal buffer offset
const int   USICNUM         =13;    // Which usic card
const int   LASTCAB         =14;    // The last cab assigned

#define getBit(buffer, address, bit) \
	(( (buffer) [ (address) ] >> (bit) ) & 1)
#define isOccupied(buffer, block) \
	(getBit( (buffer) , blockbuf[ (block) ][DETBUF], blockbuf[ (block) ][DETS]) == OCCUPIED)


#define TurnoutPos(buffer, turn) \
    	(((buffer)[turnout[(turn)][POSITION_PORT]] & \
		(0x01 << turnout[(turn)][POSITION_BITS])) >= 1) ? \
			REVERSE : NORMAL;
#define TurnoutMan(buffer, turn) \
	(((buffer)[turnout[(turn)][MANUAL_PORT]] & \
	  	(0x01 << turnout[(turn)][MANUAL_BITS])) == 0) ? \
			UNLOCKED : LOCKED;

const int   CONTROL_PORT    = 0;  //turnout array indices
const int   CONTROL_BITS    = 1;
const int   POSITION_PORT   = 2;
const int   POSITION_BITS   = 3;
const int   MANUAL_PORT     = 4;
const int   MANUAL_BITS     = 5;
// The following array positions are manually set by the code
const int   POSITION        = 6; // 0 = norm, 1 = reversed
const int   MANUAL_POSITION = 7; // 0 = lock, 1 = unlocked (manual)
const int   LOCK	    = MANUAL_POSITION;

const int   REVSPORT        = 0;  // Pointers int throttle array
const int   REVSOFF         = 1;
const int   PB_PORT         = 2;
const int   PB_OFF          = 3;
const int   LED_PORT        = 4;
const int   LED_OFF         = 5;
const int   T_DIR           = 6;
const int   T_USIC          = 7;
const int   T_EM_SPEED      = 8;

const int   DCC  	    = 7;

typedef struct Options_t {
	int	showswitchstands;
	int	showloopcounter;
	int	showblocksignals;
	int	showunderground;

	int	nochubb;
	int	debug_screen;
	int	log_screen;
	int	cardio_screen;
	int	cardio_card;

	int	assignment_debug;
	int	turnout_debug;
	int	detector_debug;
	int	cabreassign;
	Boolean	autoquit;
} Options;


#define LOGFILE "rr.log"

struct LogQ {
	struct Message {
		Message *n, *p;
		char text[100];

		Message() {
			n = NULL;
			p = NULL;
			text[0] = '\0';
		}
	};
		
	Message *head, *tail;
	int modified;
	int lastmod;
	int counter;

	LogQ() {
		this->head = this->tail = NULL;
		this->counter = this->lastmod = this->modified = 0;
	}

	void LogQ::log(char *tstamp, const char *message);
};

extern LogQ *logQ;
extern Options options;

extern BYTE     *OutBuf;   // Output Buffer
extern BYTE     *InBuf;    // Input Buffer
extern BYTE     *SaveBuf;  // Copy of Output Buffer
extern int  blockbuf[NumberBlocks + 1][BBParms  ];
extern int  turnout[NumberTurnouts + 1][9];
extern int  subblockbuf[NumberSubs + 1][6];
extern int  throttle[8][9];
extern int  panel[7][13];
extern char cabnum [8][12];
extern int  Block[NumberBlocks + 1][BBParms];
extern int  logflag;

#endif



