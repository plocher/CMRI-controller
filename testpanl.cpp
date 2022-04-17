#define NATIVE

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
// #include <unistd.h>

#include <iostream.h>
#include <string.h>
#include <math.h>

#include "rr.h"
#include "cards.h"
#include "globals.h"
#include "port.h"  // Port comunications
#include "usic.h"  // USIC comunications
#include "panel.h"


BYTE     *OutBuf;   // Output Buffer
BYTE     *InBuf;    // Input Buffer
BYTE     *SavBuf;  // Copy of Output Buffer

int
main(int argc, char *argv[])
{
        int y, x = 0;

	setbuf (stdout, NULL);

	printf("Chubb Assign/Drop Panel Test Program\n");

   	printf("Initializing Card Arrays\n");

	Cards *IOcards = new Cards();

   	printf("Allocating Buffers\n");

	OutBuf = new BYTE[IOcards->OutputBufferSize()];
	SavBuf = new BYTE[IOcards->OutputBufferSize()];
	InBuf  = new BYTE[IOcards->InputBufferSize()];

	for (x=0; x < IOcards->NumberOutputPorts(); x++) {
		OutBuf[x] = 0;
		SavBuf[x] = 0;
	}
	for (x=0; x < IOcards->NumberInputPorts(); x++) {
		InBuf[x]  = 0;
	}

	printf("Initializing USIC\n");

	USIC usic(IOcards, 0, Port::COM1, 9600);

	y = 0;
	while (1) {
	    if (y == 0) {
	        printf("Panel 1 Panel 2 Panel 3 Panel 4 Panel 5 Panel 6\n");
	        printf("Dblk:CA Dblk:CA Dblk:CA Dblk:CA Dblk:CA Dblk:CA\n");
	        printf("------- ------- ------- ------- ------- -------\n");
	    }
	    y = (y + 1 % 10);

	    usic.Read(InBuf);

	    for (x = 1; x <= 6; x++) {
		    // Get panel settings
		    int mainline = PANEL_GET_MAIN(InBuf, panel[x]);
		    int block    = PANEL_GET_BLOCK(InBuf, panel[x]);
		    int cab      = PANEL_GET_CAB(InBuf, panel[x]);
		    int drop     = InBuf[PANEL_DROP] & panel[x][PANEL_BUTTON];
		    int assign   = InBuf[PANEL_ASSIGN] & panel[x][PANEL_BUTTON];

		    if (mainline + block + cab + drop + assign == 0) {
			    printf("- n/c -");
		    } else {
			    if (drop) printf("D"); else printf(" ");
			    printf("%1d%2d:%1d ", mainline, block, cab);
			    if (assign) printf("A"); else printf(" ");
		    }
		    printf(" ");
	    }
	    if (kbhit()) break;
	    printf("\n");
	}

	printf("Done\n");
}


