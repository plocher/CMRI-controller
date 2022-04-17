// @(#)usic.cc	1.2 99/04/11 SBHRS

// usic functions

// #define DEBUG_USIC 1
// #define DEBUG_USIC_DETAILS
// #define DEBUG_USIC_ERRORS

#include "port.h"
#include "usic.h"
#include "util.h"

#if defined(DEBUG_USIC) || defined(DEBUG_USIC_DETAILS) || defined(DEBUG_USIC_ERRORS)

static
char *nametable[] = {
	"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
	"BS",  "HT",  "NL",  "VT",  "NP",  "CR",  "SO",  "SI",
	"DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
	"CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US",
	"SP",  "!",   "\"",  "#",   "$",   "%",   "&",   "'",
	"(",   ")",   "*",   "+",   ",",   "-",   ".",   "/",
	"0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",
	"8",   "9",   ":",   ";",   "<",   "=",   ">",   "?",
	"@",   "A",   "B",   "C",   "D",   "E",   "F",   "G",
	"H",   "I",   "J",   "K",   "L",   "M",   "N",   "O",
	"P",   "Q",   "R",   "S",   "T",   "U",   "V",   "W",
	"X",   "Y",   "Z",   "[",   "\\",  "]",   "^",   "_",
	"`",   "a",   "b",   "c",   "d",   "e",   "f",   "g",
	"h",   "i",   "j",   "k",   "l",   "m",   "n",   "o",
	"p",   "q",   "r",   "s",   "t",   "u",   "v",   "w",
	"x",   "y",   "z",   "{",   "|",   "}",   "~",   "DEL"   
};

char *
USIC::nameofchar(BYTE ch)
{
	static char value[10];

	if ((ch >= 0) && (ch < 128) ) {
		return nametable[ch];
	} else if (ch == USIC::FRAME) {
		return "FRAME";
	} else {
		sprintf(value, "%02x", ch);
		return value;
	}
}
#endif

USIC::USIC(
	Cards* IOcards,
	int unitNumber,
	int serialPort,
	int baudrate,
	Boolean autoquit
) : Port(serialPort, baudrate, autoquit) {

	BYTE    messagetype, initbuf[255];
	int     messagelength, i;

	// Set the USIC unit number

	USIC::ioCards        = IOcards;
	USIC::UnitNumber     = unitNumber;
	USIC::serialport     = serialPort;

#ifdef DEBUG_USIC
        printf("usic: DEBUG_USIC\n");
#endif
#ifdef DEBUG_USIC_DETAILS
        printf("usic: DEBUG_USIC_DETAILS\n");
#endif
#ifdef DEBUG_USIC_ERRORS
        printf("usic: DEBUG_USIC_ERRORS\n");
#endif
#ifdef DEBUG_USIC
        printf("usic: Unit %d, Output Ports: %d, Cardsets: %d, ",
		unitNumber, this->ioCards->NumberOutputPorts(), this->ioCards->NumberCardSets());
        if (serialport == Port::COM1) {
        	printf("COM1");
	} else if (serialport == Port::COM2) {
        	printf("COM2");
	} else if (serialport == Port::COM3) {
        	printf("COM3");
	} else if (serialport == Port::COM4) {
        	printf("COM4");
	} else if (serialport == Port::NONE) {
        	printf("NONE");
	}
        printf(", Baud: %d, delay: %d\n", baudrate, USIC::DELAY);
#endif
	if (serialport == Port::NONE) {
		return;
	}

	// Set up the output buffer to hold the info needed to init the USIC
	// These values are derived from the protocol set up by Bruce Chubb
	// in his book.

	messagetype = 'I';  // Initialize message
	messagelength = 1;
	initbuf[messagelength++] = 'N';                                      // No CRC check
	initbuf[messagelength++] = (BYTE)(USIC::DELAY/256);                  // delay hi byte
	initbuf[messagelength++] = (BYTE)(USIC::DELAY - (initbuf[2] * 256)); // delay lo byte
	initbuf[messagelength++] = (BYTE) this->ioCards->NumberCardSets();                          // # of sets of 4 cards

	for ( i = 0; i < this->ioCards->NumberCardSets(); i++) { // loop thru card sets
		initbuf[messagelength++] = this->ioCards->CardType[i];
	}

	// send out the initialization packet
	FormPacket(initbuf, messagetype, messagelength);
}

// Form and send a packet using Dr. Chubb's protocol

void
USIC::FormPacket(BYTE outarray[], BYTE mestype, int meslength){

	BYTE    transbuf[255];          // actual tranmit buffer
	int     x = 0;            // counter for transbuf
	int     i;
#ifdef DEBUG_USIC
        char	*debugpad = "{";
#endif

	transbuf[x++] = USIC::FRAME;          //Framing byte
	transbuf[x++] = USIC::FRAME;          //Framing byte
	transbuf[x++] = USIC::STX;
	transbuf[x++] = (BYTE) (USIC::UnitNumber + USIC::OFFSET);
	transbuf[x++] = mestype;

	if (mestype != 'P'){    // If not a POLL message add data to buffer
        			// NOTE:  'P'oll Packets don't have an outarray!
		for (i = 1; i <= meslength; i++){
                    // check for escapable characters - if so, insert a DLE first
		    if (
			(outarray[i] == USIC::STX) ||
			(outarray[i] == USIC::ETX) ||
			(outarray[i] == USIC::DLE)
		       ) {
                    	transbuf[x++] = USIC::DLE;
		    }
		    transbuf[x++] = outarray[i];
		}
        }
	transbuf[x++] = USIC::ETX;        //End of packet!

#ifdef DEBUG_USIC
	printf("[FRAME|FRAME|STX|%d|\"%c\"", (int)USIC::UnitNumber, mestype);
	switch (mestype){
        case 'P':	printf("|ETX]\n");	break;
        case 'I':
        case 'T':	printf("|%02d", meslength);
        		for (i = 1; i <= meslength; i++){
                        	printf("%s%s", debugpad, binary(8, outarray[i], '_'));
                                debugpad = ":";
                        }
                        printf("}|ETX]\n");
                        break;
        }
#endif

        for (i = 0; i < x; i++) {   // Now, send the packet!
        	WriteByte(transbuf[i]);
        }
}

// To write to the USIC, simply set 'T' message type and send the
// output buffer and transmit message length 

void USIC::Write(BYTE outbuf[]) {
	if (serialport == Port::NONE) {
		return;
	}
	FormPacket(outbuf, 'T', this->ioCards->NumberOutputPorts());
}


#define MAX_RETRIES	10

// Wait for the expected character, discarding any unexpected ones
int
USIC::waitfor(BYTE waitingfor, char *message)
{
	int cc;
	int retrycount = 0;
        BYTE inbyte;

#ifdef DEBUG_USIC_DETAILS
	char *errmsg = "Read Error";
	cerr << "USIC::waitfor(" << nameofchar(waitingfor)
	     << ", " << message << ")";
#endif
	for(;;) {
		cc = ReadByte(inbyte);   // Read from the serial port until we find our byte
                if (cc == -1) {
#ifdef DEBUG_USIC_DETAILS
                	cerr << errmsg;
			errmsg = "@";
#endif
                } else if (cc == 1) { // nothing to read
#ifdef DEBUG_USIC_DETAILS
                	cerr << ".";
#endif
			if (retrycount++ > MAX_RETRIES) {
#ifdef DEBUG_USIC_DETAILS
				cerr << "\n";
#endif
				return -1;
			}
		} else {	// cc == 0
			if (inbyte != waitingfor) {
#ifdef DEBUG_USIC_DETAILS
				cerr << "[" << nameofchar(inbyte) << "]";
#endif
			} else {
#ifdef DEBUG_USIC_DETAILS
				cerr << "OK\n";
#endif
				break;
			}
                }
        }
	return 0;
}

// Require the next character to be the expected one, error otherwise
int
USIC::expect(BYTE waitingfor, char* message)
{
	int cc;
        BYTE inbyte;

#ifdef DEBUG_USIC_DETAILS
	cerr << "USIC::expect(" << nameofchar(waitingfor)
	     << ", " << message << ")";
#endif
	for(;;) {
		cc = ReadByte(inbyte);
		if (cc == -1) {
#ifdef DEBUG_USIC_ERRORS
			cerr << ": READ ERROR\n";
#endif
			return -1;
		} else if (cc == 1) { // nothing to read
#ifdef DEBUG_USIC_DETAILS
                	cerr << ".";
#endif
			continue;
		} else { // cc == 0
			if ((inbyte) != waitingfor) {
#ifdef DEBUG_USIC_ERRORS
				cerr << "Unexpected: " << nameofchar(inbyte) << "\n";
#endif
				return -1;
			} else {
#ifdef DEBUG_USIC_DETAILS
				cerr << "OK\n";
#endif
				return 0;
			}
		}
	}
}


void
USIC::Read(BYTE inbuf[]) {
	int  i, cc;
	BYTE inbyte;
	int portcount = this->ioCards->NumberInputPorts();

#ifdef DEBUG_USIC_DETAILS
	char *sep = "";
        printf("USIC::Read(inbuf, %d)\n", portcount);
#endif
	if (serialport == Port::NONE) {
		return;
	}

RESTART:

	for (i = 1; i <= portcount; i++ ) {
        	inbuf[i] = 0;
        }

        // Send out a POLL message to get the USIC to respond with a state dump

	FormPacket(NULL, 'P', 0);

        // Read result packet from serial port

	cc = waitfor(USIC::STX, "Packet Header");
	if (cc == -1) {
		goto RESTART;
	}
	if (-1 == expect(USIC::UnitNumber + USIC::OFFSET, "USIC address")) {
		goto RESTART;
	}
	if (-1 == expect('R', "USIC Reply Packet Type")) {
		goto RESTART;
	}

#ifdef DEBUG_USIC_DETAILS
	cerr << "in[" << portcount << "]={";
#endif
	for(i = 1; i <= portcount; i++) { // Array of data from USIC
		cc = ReadByte(inbyte);
		if ( (cc == -1) ) {
#ifdef DEBUG_USIC_ERRORS
        		cerr << "USIC READ ERROR: couldn't read data payload\n";
#endif
                	goto RESTART;
		} else if (
		    (inbyte == USIC::STX) ||
		    (inbyte == USIC::ETX)
		   ) {
#ifdef DEBUG_USIC_ERRORS
			cerr << "USIC READ ERROR: truncated packet\n"
			     << "(Found unescaped STX/ETX: "
			     << (int)inbyte << ")\n";
#endif
			goto RESTART;
		}
		if (inbyte == USIC::DLE) {    // Received DLE so read next byte
			cc = ReadByte(inbyte);
			if ( (cc == -1) ) {
#ifdef DEBUG_USIC_ERRORS
        			cerr << "USIC READ ERROR: couldn't read quotedd packet data\n";
#endif
                		goto RESTART;
        		}
                }
		inbuf[i] = inbyte;      // store as a valid byte in buffer
#ifdef DEBUG_USIC_DETAILS
		cerr << sep << binary(8, inbyte, '_');
		sep=",";
#endif
	}
#ifdef DEBUG_USIC_DETAILS
	cerr << "}\n";
#endif
	if (-1 == expect(USIC::ETX, "USIC end-of-packet")) {
		goto RESTART;
	}
#ifdef DEBUG_USIC
        char	*debugpad = "{";
	printf("[FRAME|FRAME|STX|%d|\"R\"", (int)USIC::UnitNumber);
        printf("|%02d", portcount);
	for (i = 1; i <= portcount; i++){
		printf("%s%s", debugpad, binary(8, inbuf[i], '_'));
		debugpad = ":";
	}
	printf("}|ETX]\n");
#endif
}


