// @(#)autoad.cc	1.3 99/08/27 SBHRS

// autoad  cab assign and drop routine

#include    "rr.h"
#include "globals.h"

static void
do_DropCab(BYTE *OutBuf, int block)  {
	blockbuf[block][LASTCAB] = NOCAB;
	blockbuf[block][CURCAB]  = NOCAB;   // drop cab from block

	// remove cab from OutBuf which drops the Cab Card relay
	OutBuf[blockbuf[block][OBUF]] |= (0x07 << blockbuf[block][OBUFS]);
}

static void
do_AssignCab(BYTE *OutBuf, int block, int cab)  {
	blockbuf[block][LASTCAB] = cab;
	blockbuf[block][CURCAB]  = cab;

	// Add cab to OutBuf which sets the Cab Card relay
	OutBuf[blockbuf[block][OBUF]] ^= (cab << blockbuf[block][OBUFS]); 
}

void
dropblock(BYTE *OutBuf, int block)
{
	int cab = blockbuf[block][CURCAB];
	do_DropCab(OutBuf, block);

	if (options.assignment_debug == TRUE) {
	    char errstr[80];
	    sprintf(errstr, "Dropping cab %d from block %d", cab, block);
	    logmessage(errstr);
	}
}

void
assignblock(BYTE *OutBuf, int block, int cab)
{
	do_AssignCab(OutBuf, block, cab);
	if (options.assignment_debug == TRUE) {
		char errstr[80];
		sprintf(errstr, "Assigning cab %d to block %d", (int)cab, block);
		logmessage(errstr);
	}
}

static void
autodropblock(BYTE *OutBuf, int block)
{
	int lastcab = blockbuf[block][LASTCAB];
	int cab = blockbuf[block][CURCAB];
	do_DropCab(OutBuf, block);
	blockbuf[block][LASTCAB] = lastcab;

	if (options.assignment_debug == TRUE) {
	    char errstr[80];
	    sprintf(errstr, "AutoDropping cab %d from block %d", cab, block);
	    logmessage(errstr);
	}
}

static void
autoassignblock(BYTE *OutBuf, int block, int cab) {
	do_AssignCab(OutBuf, block, cab);
	if (options.assignment_debug == TRUE) {
		char errstr[80];
		sprintf(errstr, "AutoAssigning cab %d to block %d", (int)cab, block);
		logmessage(errstr);
	}
}


static
void assignAdvanceBlock(BYTE *OutBuf, BYTE *InBuf, int ab, int nab, int cab) {

	// If next block does not exist
	// OR If next block is OCCUPIED
	// OR If next block has a cab assigned
	// do nothing
	if ((ab == NOBLOCK) ||
	    isOccupied(InBuf, ab) ||
	    (blockbuf[ab][CURCAB] != NOCAB)) {
	    return;
	}

	// We also wish to keep a buffer block between assigned blocks.
	// if this next next block exists, it also should not be
	// occupied or be assigned...
	if ((nab == NOBLOCK) ||
	    isOccupied(InBuf, nab) ||
	    (blockbuf[nab][CURCAB] != NOCAB)) {
		return;
	}
	// Finally, if all OK, assign block
	autoassignblock(OutBuf, ab, cab);
}

void
autoad(BYTE *InBuf, BYTE *OutBuf, BYTE *SavBuf)
{
	int     i,
		approachblock,
		nextblock,
		advanceblock,
		nextadvanceblock;
	// char m[80];

	//	approachblock
	// 	=============== ======= ========
	//	NextWest	i ->	NextEast

	// First, do block drops

	for(i = 1; i <= NumberBlocks; i++) {
		int currentcab = blockbuf[i][CURCAB];
	        int nextwest = blockbuf[i][NEXTWEST];
	        int nexteast = blockbuf[i][NEXTEAST];

		if (isOccupied(InBuf, i))	{ continue; }	// If block occupied, do nothing
		if (currentcab == NOCAB)	{ continue; }	// If no cab assigned, do nothing

		if (currentcab == DCC) {
			if (isOccupied(InBuf, nextwest) ||
			    isOccupied(InBuf, nexteast)) {
				continue;
			}
			autodropblock(OutBuf, i);
		} else {
			// Get direction from throttle,
			blockbuf[i][DIRECTION] = throttle[currentcab][T_DIR];

			// and assign approachblock to block in the opposite direction
			if (blockbuf[i][DIRECTION] == WESTBOUND) {
			    approachblock = nexteast;
			    nextblock = nextwest;
			} else {
			    approachblock = nextwest;
			    nextblock = nexteast;
			}

			if ( (approachblock == NOBLOCK)			// previous block does not exist
			   ||(!isOccupied(InBuf, approachblock))	// previous block is CLEAR
			   ||(currentcab != blockbuf[approachblock][CURCAB])) { // previous block has different cab
				if ( (nextblock != NOBLOCK) &&
				     (isOccupied(InBuf, nextblock)) &&
			             (currentcab == blockbuf[nextblock][CURCAB])) {
					autodropblock(OutBuf, i);
					blockbuf[i][LASTCAB] = NOCAB;
					// sprintf(m, "autodropped forcefully: %d", i);
					// logmessage(m);
				} else {
					autodropblock(OutBuf, i);
				}
			}
		}
	}

///////////////////////////////////////////////////////////////////////////
// Now do block assignments
///////////////////////////////////////////////////////////////////////////

	for(i = 1; i <= NumberBlocks; i++) {
		int currentcab = blockbuf[i][CURCAB];
		int lastcab    = blockbuf[i][LASTCAB];
		int nextwest   = blockbuf[i][NEXTWEST];
		int nexteast   = blockbuf[i][NEXTEAST];
		int nextwestblock = (nextwest == NOBLOCK) ?  NOBLOCK : blockbuf[nextwest][NEXTWEST];
		int nexteastblock = (nexteast == NOBLOCK) ?  NOBLOCK : blockbuf[nexteast][NEXTEAST];


		if (!isOccupied(InBuf, i))	{ continue; } // If block clear, do nothing
		if (currentcab == NOCAB) {
		    if (options.cabreassign == TRUE && lastcab != NOCAB) {
			// reassign to last known cab...
			autoassignblock(OutBuf, i, lastcab);
			currentcab = lastcab;
		    } else {
			continue;  // If no cab assigned, do nothing
		    }
		}

		if (currentcab == DCC) {
		    assignAdvanceBlock(OutBuf, InBuf, nextwest, nextwestblock, DCC);
		    assignAdvanceBlock(OutBuf, InBuf, nexteast, nexteastblock, DCC);
		} else {
			// Get direction and assign advanceblock
			if (throttle[currentcab][T_DIR] == WESTBOUND) {
			    blockbuf[i][DIRECTION] = WESTBOUND;
			    assignAdvanceBlock(OutBuf, InBuf, nextwest, nextwestblock, currentcab);
			} else {
			    blockbuf[i][DIRECTION] = EASTBOUND;
			    assignAdvanceBlock(OutBuf, InBuf, nexteast, nexteastblock, currentcab);
			}
		}
	}
}

//check sub-blocks.  If occupied, set main block occupancy bit.
void CheckSubBlock()
{
	int     i;
	int     block;

	for( i = 1; i <= NumberSubs; i++) {
		if (((InBuf[subblockbuf[i][0]] >> subblockbuf[i][1]) & 1) == 1) {
			if(turnout[subblockbuf[i][2]][POSITION] == NORMAL) {
				block = subblockbuf[i][3];
			} else {
				block = subblockbuf[i][4];
			}
			if(block != NOBLOCK) {
				InBuf[blockbuf[block][DETBUF]] |= (1 << blockbuf[block][DETS]);
			}
		}
	}
}

// j = blocknum
//

