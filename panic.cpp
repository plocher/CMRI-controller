// @(#)panic.cc	1.2 99/08/27 SBHRS

// Panic:  stops all trains and waits for a panic clear


#include    "rr.h"
#include    "globals.h"
#include    "port.h"        // Port comunications
#include    "usic.h"        // USIC comunications

static void
init_local(BYTE LocalBuf[256])
{
	int     i;

	// *** Changes that need to persist after a panic "reset" go here ***

	// Set speeds to ZERO
	// PLOCHER:  What does this control?
	// It stomps on the Cab Relay slots for 413, 414, 415,416
	// that use [27/0], [27/3], [28/0] and [28/3]

	for (i = 25; i <= 28; i++) {
	    OutBuf[i]   = 0;                //Turn off throttles
	}

	// Make a local copy of the Output Buffer that we can write
	// it back out with the proper modifications when the panic
	// is cleared.

	for (i = 0; i <= NumberOutputPorts; i++) {
	    LocalBuf[i] = OutBuf[i];
	}

	// *** Changes that "go away" after a panic "reset" go here ***

	// Drop all throttles by breaking cab connections
	for (i = 1; i <= 24; i++) {
	    OutBuf[i] |= 0x3f;
	}
	for (i = 30; i <= 60; i++) {
	    OutBuf[i] |= 0x3f;   //break white cab connections
	}

	OutBuf[133] = LocalBuf[133] = 0;    // turn off white cabs
	OutBuf[134] = LocalBuf[134] = 0;

	for (i = 91; i <= 132; i++) {       // turn all the block signals on
		OutBuf[i] = 255;            // to indicate a panic
	}

	// *** End of "panic state changes" ***
}

static void
blink_signals(USIC *usic)
{
	static int blink = 0;
	int i;

        for (i = 91; i <= 132; i++) {  // blink block signals
                OutBuf[i] = blink;       // Turn them ON or OFF
        }
        if (options.nochubb == FALSE) usic->Write(OutBuf);
	update_screen(usic);
        blink = ~blink;
	sleep(1);
}

void panic(USIC *usic)
{
	BYTE    LocalBuf[256];
	char    screen_buffer[4096];
	int     i;

	if (((InBuf[8] >> 7) & 1) == 0) {   // check for PANIC button push
	    return;                         // ... NOT pushed
	}

	logmessage("*****PANIC*****");
	clrdspmnu();
	textcolor(RED);
        gotoxy( 3, 2); cprintf("P)anic Button pressed on layout: ");
        gotoxy( 3, 3); cprintf("    Waiting for PanicReset from layout");
	textcolor(BLACK);

	init_local(LocalBuf);		// reset throttles etc

	do {				// loop while blinking block signals
	    blink_signals(usic);
	    usic->Read(InBuf);
	} while (((InBuf[10] >>6) & 0x01) == 0);  //Check for panic reset button

	// Restore railroad to the "pre-panic" state (with throttles at zero)
	for (i = 1; i <= NumberOutputPorts; i++) {
	    OutBuf[i] = LocalBuf[i];
	}
        if (options.nochubb == FALSE) usic->Write(OutBuf);

	logmessage("***** Panic Cleared *****"); // inform log file
	clrdspmnu(); //clear dispatcher entry screen
	dspmnu(); // redraw dispatcher main menue
}


void key_panic(USIC *usic)
{
	int     i;
	BYTE    LocalBuf[256];
	char    screen_buffer[4096];

	logmessage("*****KBD***PANIC*****");

	clrdspmnu();
	textcolor(RED);
        gotoxy( 3, 2); cprintf("P)anic: ");
        gotoxy( 3, 3); cprintf("    Press Any Key");
	textcolor(BLACK);

	init_local(LocalBuf);	// reset throttles etc

	do {
	    blink_signals(usic);
	} while (!kbhit());

	//Now, restore railroad (but with throttles at zero)
	for (i = 1; i <= NumberOutputPorts; i++) {
	    OutBuf[i] = LocalBuf[i];
	}
        if (options.nochubb == FALSE) usic->Write(OutBuf);

	logmessage("***** Keyboard Panic Cleared *****");

	clrdspmnu(); //clear dispatcher entry screen
	dspmnu(); // redraw dispatcher main menue
}

