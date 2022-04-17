// @(#)usic.h	1.1 98/12/22 SBHRS

#ifndef _USIC_H_
#define _USIC_H_

#include "rr.h"
#include "cards.h"
#include "port.h"

/*
 * The format of the Chubb USIC protocol is:
 *      Byte 1 = 255
 *      Byte 2 = 255
 *      Byte 3 = STX  Start of text character
 *      Byte 4 = USICunitNumber + OFFSET  (the serial card number plus 65)
 *      Byte 5 = message type  I = initialize
 *                             P = poll the boards for input values and
 *                                 send back
 *                             R = Reply (sent by USIC only)
 *                             T = Transmit
 *      Byte 6 - (n-1) = output values for output cards
 *      Byte n = ETX  end of text character
 *
 *      If a data value equals 2 (STX), 3(ETX), or 16(DLE), then a DLE
 *      character is sent first to inform the USIC that the next character
 *      is a data value and not a STX, ETX, or DLE.
 */


// Lowest level of communicating with the USIC

class USIC : public Port
{
    private:
	int serialport;
        int UnitNumber;     // USIC interface number

        void FormPacket(BYTE outarray[], BYTE mestype, int meslength);
	char *nameofchar(BYTE ch);
        int  waitfor(BYTE waitingfor, char *message);
        int  expect(BYTE waitingfor, char *message);
        BYTE Read1Byte(void);

    public:
	USIC(Cards* IOcards,
	      int unitNumber = 0,
	      int port = Port::COM1,
	      int baud = 9600,
	      Boolean autoquit= FALSE);

        void Write(BYTE outbuf[]);
        void Read(BYTE inbuf[]);
        // int OutputPorts() { return ioCards->NumberOutputPorts(); }
        // int InputPorts() { return ioCards->NumberInputPorts(); }
	Cards *ioCards;

    enum {
		  DELAY = 200,	// Delay value for interface to keep the
                                // interface from overrunning the CPU.
				// About 10uS per unit, 6000 = 60ms
		  OFFSET = 65	// Offset value for individual interfaces.
	       };

    enum {
		  STX = 2,    // Start-of-TeXt
    		  ETX = 3,    // End-of-TeXt
    		  DLE = 16,    // Data Link Escape
                  FRAME = 255	// Framing Byte
		};

};

extern char *binary(int max, unsigned long bits);


#endif

