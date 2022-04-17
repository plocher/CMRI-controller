// @(#)signal.cc	1.3 99/08/27 SBHRS

// run the lineside signals

#include "rr.h"
#include "globals.h"
#include "port.h"         // Port comunications


// See sbhrs.cc ...
// This code assumes all signals are contiguous
//                         91=red 92=yellow 93=green etc, 8 per card
// West signals/West main  91- 93  94- 96  97- 99
// East signals/East main 100-102 103-105 106-108
// East signals/West main 109-111 112-114 115-117
// West signals/East main 118-120 121-123 124-126
// -unused-               127-129
// Staging		  130-132


static
void do_ABS(BYTE *OutBuf, BYTE *InBuf, int SIGBASE, int SIGOFF, int b0, int b1, int b2, int b3)
{
	int aspect = RR_RED;	// assume red condition
	static int flash = 1;	// flasher state on hand throttle

	flash *= -1;

	// don't process bogus signal data
        if (blockbuf[b0][SIGBASE] == 0) {
		return;
	}

	// turnout set against
        // or something in next block
        // if current occupied, and next is different cab
	// if current not occupied and next not assigned
        // if next next block is occupied or already assigned
        if ( (b1 == NOBLOCK) ||
             (isOccupied(InBuf, b1)) ||
             ((isOccupied(InBuf, b0)) && (blockbuf[b0][CURCAB] != blockbuf[b1][CURCAB])) ||
             ((!isOccupied(InBuf, b0)) && (blockbuf[b1][CURCAB] != NOCAB)) ||
             (isOccupied(InBuf, b2)) ||
             (blockbuf[b2][CURCAB] != NOCAB)
	     ) {
		aspect = RR_RED;
		// TODO: need to check to see if spur turnout is reversed
        } else
	    // if next next block is occupied or already assigned
	    if  ( (isOccupied(InBuf, b3)) ||
              (blockbuf[b3][CURCAB] != NOCAB) ) {
                aspect = RR_YELLOW;
        } else { //allow green aspect
		aspect = RR_GREEN;
	}

	// Approach Lighting would go here... aspect = RR_NONE...

        // set aspect
	if (aspect == RR_RED) {
	    // set RED signal and turn on hand throttle LED
            OutBuf[blockbuf[b0][SIGBASE]+0] |= (1 << blockbuf[b0][SIGOFF]);
	    OutBuf[blockbuf[b0][SIGBASE]+1] &= ~(1 << blockbuf[b0][SIGOFF]);
	    OutBuf[blockbuf[b0][SIGBASE]+2] &= ~(1 << blockbuf[b0][SIGOFF]);
	    OutBuf[throttle[blockbuf[b0][CURCAB]][LED_PORT]] |= (1<<throttle[blockbuf[b0][CURCAB]][LED_OFF]);
	} else if(aspect == RR_YELLOW) { //turn on yellow aspect
	    OutBuf[blockbuf[b0][SIGBASE]+0] &= ~(1 << blockbuf[b0][SIGOFF]);
            OutBuf[blockbuf[b0][SIGBASE]+1] |= (1 << blockbuf[b0][SIGOFF]);
	    OutBuf[blockbuf[b0][SIGBASE]+2] &= ~(1 << blockbuf[b0][SIGOFF]);
            //blink hand throttle LED
            if (flash == 1) {   //turn led off
                OutBuf[throttle[blockbuf[b0][CURCAB]][LED_PORT]] &= ~(1<<throttle[blockbuf[b0][CURCAB]][LED_OFF]);
            } else {   //turn led on
                OutBuf[throttle[blockbuf[b0][CURCAB]][LED_PORT]] |= (1<<throttle[blockbuf[b0][CURCAB]][LED_OFF]);
            }
        } else if (aspect == RR_GREEN) { // turn on green lineside aspect
	    OutBuf[blockbuf[b0][SIGBASE]+0] &= ~(1 << blockbuf[b0][SIGOFF]);
	    OutBuf[blockbuf[b0][SIGBASE]+1] &= ~(1 << blockbuf[b0][SIGOFF]);
            OutBuf[blockbuf[b0][SIGBASE]+2] |= (1 << blockbuf[b0][SIGOFF]);
	    OutBuf[throttle[blockbuf[b0][CURCAB]][LED_PORT]] &= ~(1<<throttle[blockbuf[b0][CURCAB]][LED_OFF]);
        } else {
	    OutBuf[blockbuf[b0][SIGBASE]+0] &= ~(1 << blockbuf[b0][SIGOFF]);
	    OutBuf[blockbuf[b0][SIGBASE]+1] &= ~(1 << blockbuf[b0][SIGOFF]);
	    OutBuf[blockbuf[b0][SIGBASE]+3] &= ~(1 << blockbuf[b0][SIGOFF]);
	    OutBuf[throttle[blockbuf[b0][CURCAB]][LED_PORT]] &= ~(1<<throttle[blockbuf[b0][CURCAB]][LED_OFF]);
	}
}

void do_signals(void)
{
    int     i;
    int     nexteast, nextwest, secondeast, secondwest,
            thirdeast, thirdwest;

    // set the lineside signals

    for(i = 1; i <= NumberBlocks; i++) {
        nexteast = blockbuf[i][NEXTEAST];
        if(nexteast != NOBLOCK) {
            secondeast = blockbuf[nexteast][NEXTEAST];
            if(secondeast != NOBLOCK) {
                thirdeast = blockbuf[secondeast][NEXTEAST];
            } else {
                thirdeast = NOBLOCK;
	    }
        } else {
            secondeast = NOBLOCK;
	}

        nextwest= blockbuf[i][NEXTWEST];
        if(nextwest!= NOBLOCK) {
            secondwest= blockbuf[nextwest][NEXTWEST];
            if(secondwest!= NOBLOCK) {
                thirdwest= blockbuf[secondwest][NEXTWEST];
            } else {
                thirdwest = NOBLOCK;
	    }
        } else {
            secondwest = NOBLOCK;
	}
	// LEAVE ASPECT RED IF

	do_ABS(OutBuf, InBuf, ESIGBASE, ESIGOFF, i, nexteast, secondeast, thirdeast);
	do_ABS(OutBuf, InBuf, WSIGBASE, WSIGOFF, i, nextwest, secondwest, thirdwest);
    }
}

