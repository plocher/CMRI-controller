// @(#)port.h	1.1 98/12/22 SBHRS

#ifndef _PORT_H_
#define _PORT_H_
#include "rr.h"

// Lowest level of talking over the rs232 lines

class Port
{
    private:
        int     portaddress;
        int     baud;
	Boolean	autoexit;

    public:
        Port(int portnumber, int baud, Boolean autoquit);
        int  ReadByte(BYTE& inbyte);
        void WriteByte(BYTE& outbyte);
	void setAutoexit(Boolean b);
	Boolean Autoexit(void);

    enum {
	          NONE = 0,
		  COM1 = 1,
	          COM2 = 2,
                  COM3 = 3,
                  COM4 = 4
	       };

};

#endif

