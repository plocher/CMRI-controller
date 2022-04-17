// @(#)assigndr.cc	1.3 99/08/27 SBHRS

// assigndrop  manual cab assign and drop routine

#include "rr.h"
#include "globals.h"
#include "panel.h"

void
assigndrop(BYTE *InBuf, BYTE *OutBuf, BYTE *SavBuf)
{
	int         i, error, block, detected;
	int 	    i1, i2, s1, s2;
	BYTE        mainline, cab;
	char        errstr[255];
	char        logstr[255];
	time_t      now;        
	// time structure


	    // set-up error reporting
	error = FALSE;
	    //Start with a blank string
	errstr[0] = '\0';
	    // First, see if anything has changed
	i1 = InBuf [PANEL_ASSIGN] & 0x3F;
	s1 = SavBuf[PANEL_ASSIGN] & 0x3F;
	i2 = InBuf [PANEL_DROP] & 0x3F;
	s2 = SavBuf[PANEL_DROP] & 0x3F;

	if ( (i1 == s1) && (i2 == s2) ) {
		return;
	}

	// Now, look at the buttons
	for ( i = 1; i <= 6; i++) {
		// Turn on beeper
		PANEL_SET_BEEP(OutBuf, panel[i]);

		// Read in panel settings
		mainline = PANEL_GET_MAIN(InBuf, panel[i]);
		block    = PANEL_GET_BLOCK(InBuf, panel[i]);
		cab      = PANEL_GET_CAB(InBuf, panel[i]);

		// If block selection is invalid, turn beeper off
		// However, att his time, all blocks are considered valid...
		if (block > 0xff) {
			error = TRUE;
			sprintf(errstr, "Invalid block number on panel %d: %d\n",
				i, block);
			logmessage(errstr);
		}
		// adjust for mainline if needed
		switch (mainline) {
		case 1:  block +=  0; break;
		case 2:  block += 24; break;
		case 3:  block += 64; break;
		case 4:  block += 48; break;
		case 5:
		default:
			 sprintf(errstr,
			    "Invalid mainline on panel %d: %d\n",
			    i, mainline);
			 logmessage(errstr);
			 /* FALLTHROUGH */
		case 0:
		         error = TRUE;
		}

		// If cab selection is invalid, turn beeper off
		if (cab >= 8) {
			error = TRUE;
			sprintf(errstr, "Invalid cab number on panel %d: %d\n",
				i, cab);
			logmessage(errstr);
		}
		if (error) {
			error = 0;
			PANEL_RESET_BEEP(OutBuf, panel[i]);
			continue;
		}

		// if assign button pushed ...
		if (InBuf[PANEL_ASSIGN] & panel[i][PANEL_BUTTON]) {     

			detected = InBuf[blockbuf[block][DETBUF]] >> blockbuf[block][DETS];
			detected = detected & 0x01;

			// because the buffer contains the compliment of the
			//  needed bit pattern, to set the correct cab first
			//  shift the cab to the correct bit position (left
			//  shift either 0 or 3 bits) then exclusive-or it
			//  with the existing contents of the buffer.

			if ((blockbuf[block][CURCAB] == 0 ) && (detected)) {
				OutBuf[blockbuf[block][OBUF]] ^= (cab << blockbuf[block][OBUFS]);
				blockbuf[block][CURCAB] = cab;
				blockbuf[block][LASTCAB] = cab;
				if (options.assignment_debug == TRUE) {
					sprintf(logstr,
					    "Manually assigning cab %d to block %d at panel %d",
					    (int)cab, (int)block, i);
					logmessage(logstr);
				}
			} else {
				logmessage("ERROR: Block already assigned");
			}
		}
		// or if drop button pushed ...
		if (InBuf[PANEL_DROP] & panel[i][PANEL_BUTTON]) {     

			// set Block[block][CAB] to 0
			//  To zero it, shift an 0x07 and OR it with the contents.

			if (blockbuf[block][CURCAB] != 0) {
				blockbuf[block][CURCAB] = NOCAB;
				blockbuf[block][LASTCAB] = NOCAB;
				OutBuf[blockbuf[block][OBUF]] |= (0x07 << blockbuf[block][OBUFS]);
				if (options.assignment_debug == TRUE) {
					sprintf(logstr,
					    "Manually dropping cab %d from block %d at panel %d",
					    (int)cab, (int)block, i);
					logmessage(logstr);
				}
			} else {
				sprintf(logstr, "ERROR: Can not drop an unassigned block (Panel %d, block %d)\n",
					i, (int)block);
				logmessage(logstr);
			}
		}
		PANEL_RESET_BEEP(OutBuf, panel[i]);
	}
}

