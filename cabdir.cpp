// @(#)cabdir.cc	1.2 99/08/27 SBHRS

// Checkcabdirections does!
// Start:    3/16/94


#include "rr.h"
#include "globals.h"

void checkcabdirections(void)
{
	int     i;
	int	dirport, dirbit;
	int	changed = 0;

	for(i = 1; i <= NumberCabs; i++) {
		int oldbit, newbit;

		dirport = throttle[i][REVSPORT];
		dirbit  = throttle[i][REVSOFF];

		// oldbit = SaveBuf[dirport] & (0x01 << dirbit);
		newbit = InBuf  [dirport] & (0x01 << dirbit);

	        // if (oldbit != newbit) {
		// 	changed = 1;
		// }

		if (newbit) {
			throttle[i][T_DIR] = EASTBOUND;
		} else {
			throttle[i][T_DIR] = WESTBOUND;
		}
	}
}

