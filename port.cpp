// @(#)port.cc	1.3 99/08/18 SBHRS

// port functions


// #define DEBUG_PORT

#include "rr.h"
#include "port.h"

#ifndef MSDOS
#	include <sys/fcntl.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <fcntl.h>
#	include <unistd.h>
#	include <stropts.h>
#	include <sys/termios.h>
#	include <errno.h>
#endif



Port::Port(int serialport, int baudrate, Boolean autoquit)
{

#ifdef MSDOS
	unsigned int ms, ls;
	baud        = baudrate;
	autoexit    = autoquit;

	//	Initialize the serial hardware so we can talk to the
	//	USIC.  ReadByte and WriteByte depend on this being successful

	// NO CHUBB HARDWARE TO TALK TO
	if (serialport == Port::NONE) {
		portaddress = -1;
		baud = 0x0;
		return;
	}

	switch (serialport) {
	case Port::COM1: portaddress = 0x3f8; break; // COM1 port on IBM
	case Port::COM2: portaddress = 0x2f8; break; // COM2
	case Port::COM3: portaddress = 0x3e8; break; // COM3
	case Port::COM4: portaddress = 0x2e8; break; // COM4
	}

	switch (baudrate) {
            case   150: ms = 0x03; ls = 0x00; break;
            case   300: ms = 0x01; ls = 0x80; break;
            case   600: ms = 0x00; ls =  192; break;
            case  1200: ms = 0x00; ls =   96; break;
            case  2400: ms = 0x00; ls =   48; break;
            case  4800: ms = 0x00; ls =   24; break;
            case  9600: ms = 0x00; ls = 0x0c; break;
            default:
            case 19200: ms = 0x00; ls = 0x06; break;
	}

	// setup the uart baud rate
	//	Enable baud setting
	//	Assume 81N

	outport(portaddress + 3, 128);
	outport(portaddress + 0, ls);
	outport(portaddress + 1, ms);
	outport(portaddress + 3, 3);

#ifdef DEBUG_PORT
	printf("Port 0x03%x baud %d (0x02x 0x02x)\n", portaddress, baudrate, ms, ls);
#endif
#else

	char *portname;

	switch (serialport) {
	case Port::COM1: portname = "/dev/ttya"; break;
	case Port::COM2: portname = "/dev/ttyb"; break;
	case Port::COM3: portname = "/dev/term/a"; break;
	case Port::COM4: portname = "/dev/cua0"; break;
	}

	switch (baudrate) {
            case   150: baud = B150;	break;
            case   300: baud = B300;	break;
            case   600: baud = B600;	break;
            case  1200: baud = B1200;	break;
            case  2400: baud = B2400;	break;
            case  4800: baud = B4800;	break;
            case  9600: baud = B9600;	break;
            default:
            case 19200: baud = B19200;	break;
	}

#ifdef DEBUG_PORT
	printf("Port %s baud %d\n", portname, baudrate);
#endif

	// portaddress = open(portname, O_RDWR|O_NDELAY);
	portaddress = open(portname, O_RDWR);

	if (portaddress == -1) {
		printf("Error opening port %s\n", portname);
	} else {
                int on = 1;
		struct termios ti;

		(void)setsid();
		ioctl(portaddress,TIOCSSOFTCAR, &on);
		ioctl(portaddress, TCGETS, &ti);
                ti.c_iflag = IXON;
                ti.c_oflag = 0;
                ti.c_lflag = 0;
                ti.c_cflag = baud|CS8|CREAD|CLOCAL;
                ti.c_cc[VMIN]  =  0;
                ti.c_cc[VTIME] =  5;
                ioctl(portaddress, TCSETS, &ti);
	}

#endif
}

void Port::setAutoexit(Boolean b) {
    this->autoexit = b;
}

Boolean Port::Autoexit(void) {
    return this->autoexit;
}

// Read reads one character from the port

int Port::ReadByte(BYTE& inbyte)
{
	BYTE a;
	unsigned long loopcount = 0;

	if (portaddress == -1) {
		inbyte = 3;
		return -1;
	}

#ifdef MSDOS
#ifdef DEBUG_PORT
        int lasta = -1;
        char spin[] = "-/|\\";
        char *spinp = spin;

	printf("ReadByte() ->  ");
#endif
        a = inport(portaddress + 5);	// Get status
	while ((a & 0x1) == 0) {		// loop 'til byte is ready
	    // give the port some time to respond before eating keystrokes...
            if (loopcount++ > 200000) {
		if (this->autoexit) {
		    exit(-99);
		} else if (kbhit()) {
		    int ch = getch();
		    if ((ch == 'Q') || (ch == 'q') ) {
			printf("\n<Exiting...>\n");
			exit(1);
		    }
		}
            }
#ifdef DEBUG_PORT
            printf("\b\b\b\b\b\b\b(0x%02x)",a);
            if (a != lasta) {
            	printf("      ");
                lasta = a;
            }
            printf("%c", *spinp);

            spinp++;
            if (*spinp == '\0') spinp = spin;
#endif
	    if ((a & 0x1) != 0) break;

       	    if ((a & 0x2) != 0) {
		cout << "OVERRUN\n";
                inbyte = 3; // ETX
                return -1;
            }
            a = inport(portaddress + 5);	// Get status
	}

	a = inportb(portaddress);               // get byte
	inbyte = (BYTE)a & 0x0FF;
#ifdef DEBUG_PORT
	printf("\b 0x%02x (\"%c\")\n", a, a);
#endif
	return 0;				// return OK

#else
	char ch;
	int cc;

	inbyte = 0;

	cc = read(portaddress, &ch, 1);

	if (cc > 0) {
		inbyte = (BYTE)ch & 0x0FF;
		return 0;				// return OK
	} else if (cc == -1) {
		int e = errno;

#ifdef DEBUG_PORT
		cout << "ReadError("<< dec << cc;
		cout << ", errno: " << e << ": " << strerror(e);
		cout << ")\n";
#endif
                inbyte = 3; // ETX - a safe value
                return -1;
	} else {
		// nothing available to read ...
                return 1;
	}

#endif
}

// Write a character to the port

void Port::WriteByte(BYTE& outbyte)
{
	if (portaddress == -1) {
		return;
	}
#ifdef MSDOS
	BYTE inbyte;
#ifdef DEBUG_PORT
	cout << "WriteByte(" << (int) outbyte << ") -> ";
#endif
	do {
#ifdef DEBUG_PORT
        	if (kbhit()) {
            		printf("\n<Exiting...>\n");
                	exit(1);
            	}
#endif
        	inbyte = inport(portaddress + 5);  // wait for port to be ready
	} while ((inbyte & 32) == 0);

	outport(portaddress, outbyte);
#ifdef DEBUG_PORT
	cout << "OK\n";
#endif
#else
	BYTE ch = outbyte;

	write(portaddress, &ch, 1);
#endif
}


