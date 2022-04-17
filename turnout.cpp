// @(#)turnout.cc	1.2 99/08/06 SBHRS

#include "rr.h"
#include "globals.h"
#include "port.h"         // Port comunications


void checkturnouts(void)
{
    int         i, test = 0;
    char        logstr[80];

    // First, see if anything has changed
    /*
    for (i = 12; i <= 19; i++) {
        if (InBuf[i] != SaveBuf[i]) {
            test = 1;
        }
    }
    for (i = 41; i <= 45; i++) {
        if (InBuf[i] != SaveBuf[i]) {
            test = 1;
        }
    }
    if (test == 0) {    // If InBuf = LocalBuf then nothing to do
        return;
    }
    */
    // log the reverse switch info

    for (i = 1; i <=NumberTurnouts; i++) {    // check all turnouts
	int position,       // turnout switch
            lock_state;     // manual reverse switch
	int oposition,       // turnout switch
            olock_state;     // manual reverse switch

        //First, get current turnout position from railroad
        position = TurnoutPos(InBuf, i);

	oposition = InBuf[turnout[i][POSITION_PORT]] & (0x01 << turnout[i][POSITION_BITS]);
        if (oposition >= 1) {
            oposition = REVERSE;
	} else {
	    oposition = NORMAL;
	}
	if (position != oposition) {
	    char error[100];
	    sprintf(error, "Coding Error: turnout %d: pos(%d) != opos(%d)",i,position,oposition);
	    logmessage(error);
	}

        //Now, get current manual reverse switch position from railroad
        lock_state = TurnoutMan(InBuf, i);
        olock_state = InBuf[turnout[i][MANUAL_PORT]] & (0x01 << turnout[i][MANUAL_BITS]);
        if (olock_state != 0) {
            olock_state = LOCKED;
        } else {
            olock_state = UNLOCKED;
        }
	if (lock_state != olock_state) {
	    char error[100];
	    sprintf(error, "Coding Error: turnout %d: lock(%d) != olock(%d)",i,lock_state,olock_state);
	    logmessage(error);
	}

        // See if this one has changed
	
        if ((position   != turnout[i][POSITION])  || 
            (lock_state != turnout[i][LOCK])) {
            if(options.turnout_debug == TRUE) {   //log events if needed

		sprintf(logstr, "Turnout: %d %s%s, now %s %s", i,
        		(lock_state != turnout[i][LOCK]) ?
				((lock_state == UNLOCKED) ? "Unlocked" : "Locked") : "",
			(position   != turnout[i][POSITION]) ? "Thrown" : "",
			(lock_state == UNLOCKED) ? "Unlocked" : "Locked",
			(position   == NORMAL)   ? "Normal" : "Diverging"
				);
                logmessage(logstr);
            }

	    turnout[i][LOCK] = lock_state;
            turnout[i][POSITION] = position; //update turnout position
            adjustblock(i, position);
        }
    }
}

//adjustblock sets the new values for blockbuf next east & west entries

void
adjustblock(int turnoutnumber, int direction)
{
	if(direction == REVERSE) {
		switch (turnoutnumber) {
		case 1:
			blockbuf[17][NEXTEAST] = 16;
			blockbuf[16][NEXTWEST] = 17;
			blockbuf[1][NEXTEAST] = NOBLOCK;
			break;
		case 2:
		case 3:
		case 4:
			if(turnout[2][POSITION] == REVERSE) {
				blockbuf[24 + 1][NEXTEAST] = NOBLOCK;
				if(turnout[3][POSITION] == REVERSE) {
					blockbuf[24 + 18][NEXTEAST] = 24 + 16;
					blockbuf[24 + 16][NEXTWEST] = 24 + 18;
					blockbuf[24 + 17][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[24 + 17][NEXTEAST] = 24 + 16;
					blockbuf[24 + 16][NEXTWEST] = 24 + 17;
					blockbuf[24 + 18][NEXTEAST] = NOBLOCK;
				}
			} else {
				blockbuf[24 +  1][NEXTEAST] = 24 + 16;
				blockbuf[24 + 16][NEXTWEST] = 24 +  1;
				blockbuf[24 + 17][NEXTEAST] = NOBLOCK;
				blockbuf[24 + 18][NEXTEAST] = NOBLOCK;
			}
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			if(turnout[5][POSITION] == NORMAL) {
				if(turnout[9][POSITION] == NORMAL) {
					blockbuf[ 2][NEXTEAST] =  1;
					blockbuf[ 1][NEXTWEST] =  2;
					blockbuf[17][NEXTWEST] = NOBLOCK;
				} else {
					blockbuf[ 2][NEXTEAST] = 17;
					blockbuf[17][NEXTWEST] =  2;
					blockbuf[ 1][NEXTWEST] = NOBLOCK;
				}
				if(turnout[6][POSITION] == NORMAL) {
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = 24 +  1;
						blockbuf[24 +  1][NEXTWEST] = 24 +  2;
						blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					} else {
						blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 24 + 17;
							blockbuf[24 + 17][NEXTWEST] = 24 +  2;
						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
						}
					}
				} else {
					blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
						blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
					} else {
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 24 +  1;
							blockbuf[24 +  1][NEXTWEST] = 24 +  2;
						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
						}
					}
				}
			} else {
				if(turnout[9][POSITION] == NORMAL) {
					blockbuf[ 2][NEXTEAST] = NOBLOCK;
					blockbuf[17][NEXTWEST] = NOBLOCK;
				} else {
					blockbuf[ 2][NEXTEAST] = 17;
					blockbuf[17][NEXTWEST] =  2;
				}
				blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
				if(turnout[6][POSITION] == NORMAL) {
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = 1;
						blockbuf[      1][NEXTWEST] = 24 +  2;
						blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					} else {
						blockbuf[1][NEXTWEST] = NOBLOCK;
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 24 + 17;
							blockbuf[24 + 17][NEXTWEST] = 24 +  2;

						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
						}
					}
				} else {
					blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
						blockbuf[      1][NEXTWEST] = NOBLOCK;
					} else {
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 1;
							blockbuf[      1][NEXTWEST] = 24 +  2;
						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[      1][NEXTWEST] = NOBLOCK;
						}
					}
				}
			}
			break;

		case 10: 
			blockbuf[      2][NEXTEAST] = 24 +  1;
			blockbuf[24 +  1][NEXTWEST] =       2;
			blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
			blockbuf[      1][NEXTWEST] = NOBLOCK;
			break;
		case 11:  break;
		case 12:  break;
		case 13:  break;
		case 14:  break;
		case 15:  break;
		case 16:  break;
		case 17:  break;
		case 18:  break;
		case 19:  break;
		case 20:  break;
		case 21:  break;
		case 22:  break;	// unused

		case 23:
			blockbuf[24 + 15][NEXTEAST] =      14;
			blockbuf[     14][NEXTWEST] = 24 + 15;
			blockbuf[     15][NEXTEAST] = NOBLOCK;
			blockbuf[24 + 14][NEXTWEST] = NOBLOCK;
			break;
		case 24:
			blockbuf[     16][NEXTEAST] = 24 + 15;
			blockbuf[24 + 15][NEXTWEST] =      16;
			blockbuf[24 + 16][NEXTEAST] = NOBLOCK;
			blockbuf[     15][NEXTWEST] = NOBLOCK;
			break;
		case 25:
		case 26:
			if(turnout[25][POSITION] == REVERSE) {
				blockbuf[24 + 14][NEXTEAST] = NOBLOCK;
				blockbuf[     13][NEXTWEST] = NOBLOCK;
				if(turnout[26][POSITION] == REVERSE) {
					blockbuf[48 +  8][NEXTEAST] = 24 + 13;
					blockbuf[24 + 13][NEXTWEST] = 48 +  8;
					blockbuf[     14][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[     14][NEXTEAST] = 24 + 13;
					blockbuf[24 + 13][NEXTWEST] = 14;
					blockbuf[48 +  8][NEXTEAST] = NOBLOCK;
				}
			} else {
				blockbuf[24 + 14][NEXTEAST] = 24 + 13;
				blockbuf[24 + 13][NEXTWEST] = 24 + 14;
				if(turnout[26][POSITION] == REVERSE) {
					blockbuf[48 +  8][NEXTEAST] =      13;
					blockbuf[     13][NEXTWEST] = 48 +  8;
					blockbuf[     14][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[     14][NEXTEAST] =      13;
					blockbuf[     13][NEXTWEST] =      14;
					blockbuf[48 +  8][NEXTEAST] = NOBLOCK;
				}
			}
			break; 
			// PLOCHER - missing in original???

		case 27:   
			blockbuf[24 +  8][NEXTEAST] =       7;
			blockbuf[      7][NEXTWEST] = 24 +  8;
			blockbuf[      8][NEXTEAST] = NOBLOCK;
			blockbuf[24 +  7][NEXTWEST] = NOBLOCK;
			break;
		case 28:
			blockbuf[      7][NEXTEAST] = 24 +  6;
			blockbuf[24 +  6][NEXTWEST] =       7;
			blockbuf[24 +  7][NEXTEAST] = NOBLOCK;
			blockbuf[      6][NEXTWEST] = NOBLOCK;
			break;

		case 29: break;	// unused
		case 30: break;	// unused

		case 31:
			blockbuf[24 + 10][NEXTWEST] = NOBLOCK;
			blockbuf[24 + 22][NEXTWEST] = 24 + 12;
			blockbuf[24 + 12][NEXTEAST] = 24 + 22;
			break;
		case 32:
			blockbuf[24 +  9][NEXTWEST] = 24 + 22;
			blockbuf[24 + 22][NEXTEAST] = 24 +  9;
			blockbuf[24 + 10][NEXTEAST] = NOBLOCK;
			break;

		case 33:
		case 34:
		case 35:
		case 36:
			if(turnout[33][POSITION] == NORMAL) {
				if(turnout[34][POSITION] == NORMAL) {
					if(turnout[35][POSITION] == NORMAL) {
						if(turnout[36][POSITION] == NORMAL) {
							blockbuf[48 +  6][NEXTEAST] = 48 +  7;
							blockbuf[48 +  7][NEXTWEST] = 48 +  6;
							blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
						} else {
							blockbuf[48 + 13][NEXTEAST] = 48 +  7;
							blockbuf[48 +  7][NEXTWEST] = 48 + 13;
							blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
						}
					} else {
						blockbuf[48 + 14][NEXTEAST] = 48 +  7;
						blockbuf[48 +  7][NEXTWEST] = 48 + 14;
						blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
					}
				} else {
					blockbuf[48 + 15][NEXTEAST] = 48 +  7;
					blockbuf[48 +  7][NEXTWEST] = 48 + 15;
					blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
				}
			} else {
				blockbuf[48 + 16][NEXTEAST] = 48 +  7;
				blockbuf[48 +  7][NEXTWEST] = 48 + 16;
				blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
				blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
				blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
				blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
			}
			break;
		case 37:
		case 38:
		case 39:
		case 40:
			if(turnout[37][POSITION] == NORMAL) {
				if(turnout[38][POSITION] == NORMAL) {
					if(turnout[39][POSITION] == NORMAL) {
						blockbuf[48 + 13][NEXTWEST] = 48 +  5;
						blockbuf[48 +  5][NEXTEAST] = 48 + 13;
						blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 14][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 15][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 16][NEXTWEST] = NOBLOCK;
					} else {
						blockbuf[48 + 14][NEXTWEST] = 48 +  5;
						blockbuf[48 +  5][NEXTEAST] = 48 + 14;
						blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 13][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 15][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 16][NEXTWEST] = NOBLOCK;
					}
				} else {
					blockbuf[48 + 15][NEXTWEST] = 48 +  5;
					blockbuf[48 +  5][NEXTEAST] = 48 + 15;
					blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 13][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 14][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 16][NEXTWEST] = NOBLOCK;
				}
			} else {
				blockbuf[48 + 16][NEXTWEST] = 48 +  5;
				blockbuf[48 +  5][NEXTEAST] = 48 + 16;
				blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 13][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 14][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 15][NEXTWEST] = NOBLOCK;
			}
			break;
		case 41:
		case 42:
		case 43:
		case 44:
			if(turnout[42][POSITION] == NORMAL) {
				if(turnout[41][POSITION] == NORMAL) {
					blockbuf[48 +  4][NEXTEAST] = 48 +  5;
					blockbuf[48 +  5][NEXTWEST] = 48 +  4;
					blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[48 +  9][NEXTEAST] = 48 +  5;
					blockbuf[48 +  5][NEXTWEST] = 48 +  9;
					blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
				}
			} else {
				if(turnout[43][POSITION] == NORMAL) {
					blockbuf[48 + 10][NEXTEAST] = 48 +  5;
					blockbuf[48 +  5][NEXTWEST] = 48 + 10;
					blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
					blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
				} else {
					if(turnout[44][POSITION] == NORMAL) {
						blockbuf[48 + 11][NEXTEAST] = 48 +  5;
						blockbuf[48 +  5][NEXTWEST] = 48 + 11;
						blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
						blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
					} else {
						blockbuf[48 + 12][NEXTEAST] = 48 +  5;
						blockbuf[48 +  5][NEXTWEST] = 48 + 12;
						blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
						blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					}
				}
			}
			break;
		case 45:
			blockbuf[48 +  2][NEXTEAST] = 48 +  9;
			blockbuf[48 +  9][NEXTWEST] = 48 +  2;
			blockbuf[48 +  3][NEXTWEST] = NOBLOCK;
			break;
		case 46:
		case 47:
		case 48:
			if(turnout[46][POSITION] == NORMAL) {
				blockbuf[48 +  4][NEXTWEST] = 48 +  3;
				blockbuf[48 +  3][NEXTEAST] = 48 +  4;
				blockbuf[48 + 10][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 11][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 12][NEXTWEST] = NOBLOCK;
			} else {
				if(turnout[47][POSITION] == NORMAL) {
					blockbuf[48 + 10][NEXTWEST] = 48 +  3;
					blockbuf[48 +  3][NEXTEAST] = 48 + 10;
					blockbuf[48 +  4][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 11][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 12][NEXTWEST] = NOBLOCK;
				} else {
					if(turnout[48][POSITION] == NORMAL) {
						blockbuf[48 + 11][NEXTWEST] = 48 +  3;
						blockbuf[48 +  3][NEXTEAST] = 48 + 11;
						blockbuf[48 +  4][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 10][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 12][NEXTWEST] = NOBLOCK;
					} else {
						blockbuf[48 + 12][NEXTWEST] = 48 +  3;
						blockbuf[48 +  3][NEXTEAST] = 48 + 12;
						blockbuf[48 +  4][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 10][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 11][NEXTWEST] = NOBLOCK;
					}
				}
			}
			break;
		default : 
			break;
		}
	} else { 
		// NORMAL
		    switch (turnoutnumber) {
		case 1:
			blockbuf[ 1][NEXTEAST] = 16;
			blockbuf[16][NEXTWEST] = 1;
			blockbuf[17][NEXTEAST] = NOBLOCK;
			break;
		case 2:
		case 3:
		case 4:
			if(turnout[2][POSITION] == REVERSE) {
				blockbuf[24 +  1][NEXTEAST] = NOBLOCK;
				if(turnout[3][POSITION] == REVERSE) {
					blockbuf[24 + 18][NEXTEAST] = 24 + 16;
					blockbuf[24 + 16][NEXTWEST] = 24 + 18;
					blockbuf[24 + 17][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[24 + 17][NEXTEAST] = 24 + 16;
					blockbuf[24 + 16][NEXTWEST] = 24 + 17;
					blockbuf[24 + 18][NEXTEAST] = NOBLOCK;
				}
			} else {
				blockbuf[24 +  1][NEXTEAST] = 24 + 16;
				blockbuf[24 + 16][NEXTWEST] = 24 +  1;
				blockbuf[24 + 17][NEXTEAST] = NOBLOCK;
				blockbuf[24 + 18][NEXTEAST] = NOBLOCK;
			}
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			if(turnout[5][POSITION] == NORMAL) {
				if(turnout[9][POSITION] == NORMAL) {
					blockbuf[2][NEXTEAST] = 1;
					blockbuf[1][NEXTWEST] = 2;
					blockbuf[17][NEXTWEST] = NOBLOCK;
				} else {
					blockbuf[2][NEXTEAST] = 17;
					blockbuf[17][NEXTWEST] = 2;
					blockbuf[1][NEXTWEST] = NOBLOCK;
				}
				if(turnout[6][POSITION] == NORMAL) {
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = 24 +  1;
						blockbuf[24 +  1][NEXTWEST] = 24 +  2;
						blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					} else {
						blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 24 + 17;
							blockbuf[24 + 17][NEXTWEST] = 24 +  2;
						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
						}
					}
				} else {
					blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
						blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
					} else {
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 24 +  1;
							blockbuf[24 +  1][NEXTWEST] = 24 +  2;
						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
						}
					}
				}
			} else {
				if(turnout[9][POSITION] == NORMAL) {
					blockbuf[2][NEXTEAST] = NOBLOCK;
					blockbuf[17][NEXTWEST] = NOBLOCK;
				} else {
					blockbuf[2][NEXTEAST] = 17;
					blockbuf[17][NEXTWEST] = 2;
				}
				blockbuf[24 +  1][NEXTWEST] = NOBLOCK;
				if(turnout[6][POSITION] == NORMAL) {
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = 1;
						blockbuf[1][NEXTWEST] = 24 +  2;
						blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					} else {
						blockbuf[1][NEXTWEST] = NOBLOCK;
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 24 + 17;
							blockbuf[24 + 17][NEXTWEST] = 24 +  2;

						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
						}
					}
				} else {
					blockbuf[24 + 17][NEXTWEST] = NOBLOCK;
					if(turnout[8][POSITION] == NORMAL) {
						blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
						blockbuf[1][NEXTWEST] = NOBLOCK;
					} else {
						if(turnout[7][POSITION] == NORMAL) {
							blockbuf[24 +  2][NEXTEAST] = 1;
							blockbuf[1][NEXTWEST] = 24 +  2;
						} else {
							blockbuf[24 +  2][NEXTEAST] = NOBLOCK;
							blockbuf[1][NEXTWEST] = NOBLOCK;
						}
					}
				}
			}
			break;
		case 10: 
			blockbuf[      2][NEXTEAST] =       1;
			blockbuf[24 +  1][NEXTWEST] = 24 +  2;
			blockbuf[24 +  2][NEXTEAST] = 24 +  1;
			blockbuf[      1][NEXTWEST] =       2;
			break;
		case 11: break;
		case 12: break;
		case 13: break;
		case 14: break;
		case 15: break;

		case 19: break;

		case 23:
			blockbuf[24 + 15][NEXTEAST] = 24 + 14;
			blockbuf[24 + 14][NEXTWEST] = 24 + 15;
			blockbuf[15][NEXTEAST] = 14;
			blockbuf[14][NEXTWEST] = 15;
			break;
		case 24:
			blockbuf[16][NEXTEAST] = 15;
			blockbuf[15][NEXTWEST] = 16;
			blockbuf[24 + 16][NEXTEAST] = 24 + 15;
			blockbuf[24 + 15][NEXTWEST] = 24 + 16;
			break;
		case 25:
		case 26:
			if(turnout[25][POSITION] == NORMAL) {
				blockbuf[24 + 14][NEXTEAST] = 24 + 13;
				blockbuf[24 + 13][NEXTWEST] = 24 + 14;
				if(turnout[26][POSITION] == NORMAL) {
					blockbuf[14][NEXTEAST] = 13;
					blockbuf[13][NEXTWEST] = 14;
					blockbuf[48 +  8][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[48 +  8][NEXTEAST] = 13;
					blockbuf[13][NEXTWEST] = 48 +  8;
					blockbuf[14][NEXTEAST] = NOBLOCK;
				}
			} else {
				blockbuf[24 + 14][NEXTEAST] = NOBLOCK;
				blockbuf[13][NEXTWEST] = NOBLOCK;
				if(turnout[26][POSITION] == NORMAL) {
					blockbuf[14][NEXTEAST] = 24 + 13;
					blockbuf[24 + 13][NEXTWEST] = 14;
					blockbuf[48 +  8][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[48 +  8][NEXTEAST] = 24 + 13;
					blockbuf[24 + 13][NEXTWEST] = 48 +  8;
					blockbuf[14][NEXTEAST] = NOBLOCK;
				}
			}
			break; // missing?
		case 27:
			blockbuf[8][NEXTEAST] = 7;
			blockbuf[7][NEXTWEST] = 8;
			blockbuf[24 +  8][NEXTEAST] = 24 +  7;
			blockbuf[24 +  7][NEXTWEST] = 24 +  8;
			break;
		case 28:
			blockbuf[7][NEXTEAST] = 6;
			blockbuf[6][NEXTWEST] = 7;
			blockbuf[24 +  7][NEXTEAST] = 24 +  6;
			blockbuf[24 +  6][NEXTWEST] = 24 +  7;
			break;
		case 31:
			blockbuf[24 + 12][NEXTEAST] = 24 + 10;
			blockbuf[24 + 10][NEXTWEST] = 24 + 12;
			blockbuf[24 + 22][NEXTEAST] = NOBLOCK;
			break;
		case 32:
			blockbuf[24 + 10][NEXTEAST] = 24 +  9;
			blockbuf[24 +  9][NEXTWEST] = 24 + 10;
			blockbuf[24 + 22][NEXTEAST] = NOBLOCK;
			break;
		case 33:
		case 34:
		case 35:
		case 36:
			if(turnout[33][POSITION] == NORMAL) {
				if(turnout[34][POSITION] == NORMAL) {
					if(turnout[35][POSITION] == NORMAL) {
						if(turnout[36][POSITION] == NORMAL) {
							blockbuf[48 +  6][NEXTEAST] = 48 +  7;
							blockbuf[48 +  7][NEXTWEST] = 48 +  6;
							blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
						} else {
							blockbuf[48 + 13][NEXTEAST] = 48 +  7;
							blockbuf[48 +  7][NEXTWEST] = 48 + 13;
							blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
							blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
						}
					} else {
						blockbuf[48 + 14][NEXTEAST] = 48 +  7;
						blockbuf[48 +  7][NEXTWEST] = 48 + 14;
						blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
					}
				} else {
					blockbuf[48 + 15][NEXTEAST] = 48 +  7;
					blockbuf[48 +  7][NEXTWEST] = 48 + 15;
					blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 16][NEXTEAST] = NOBLOCK;
				}
			} else {
				blockbuf[48 + 16][NEXTEAST] = 48 +  7;
				blockbuf[48 +  7][NEXTWEST] = 48 + 16;
				blockbuf[48 +  6][NEXTEAST] = NOBLOCK;
				blockbuf[48 + 13][NEXTEAST] = NOBLOCK;
				blockbuf[48 + 14][NEXTEAST] = NOBLOCK;
				blockbuf[48 + 15][NEXTEAST] = NOBLOCK;
			}
			break;
		case 37:
		case 38:
		case 39:
		case 40:
			if(turnout[37][POSITION] == NORMAL) {
				if(turnout[38][POSITION] == NORMAL) {
					if(turnout[39][POSITION] == NORMAL) {
						if(turnout[40][POSITION] == NORMAL) {
							blockbuf[48 +  6][NEXTWEST] = 48 +  5;
							blockbuf[48 +  5][NEXTEAST] = 48 +  6;
							blockbuf[48 + 13][NEXTWEST] = NOBLOCK;
							blockbuf[48 + 14][NEXTWEST] = NOBLOCK;
							blockbuf[48 + 15][NEXTWEST] = NOBLOCK;
							blockbuf[48 + 16][NEXTWEST] = NOBLOCK;
						} else {
							blockbuf[48 + 13][NEXTWEST] = 48 +  5;
							blockbuf[48 +  5][NEXTEAST] = 48 + 13;
							blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
							blockbuf[48 + 14][NEXTWEST] = NOBLOCK;
							blockbuf[48 + 15][NEXTWEST] = NOBLOCK;
							blockbuf[48 + 16][NEXTWEST] = NOBLOCK;
						}
					} else {
						blockbuf[48 + 14][NEXTWEST] = 48 +  5;
						blockbuf[48 +  5][NEXTEAST] = 48 + 14;
						blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 13][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 15][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 16][NEXTWEST] = NOBLOCK;
					}
				} else {
					blockbuf[48 + 15][NEXTWEST] = 48 +  5;
					blockbuf[48 +  5][NEXTEAST] = 48 + 15;
					blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 13][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 14][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 16][NEXTWEST] = NOBLOCK;
				}
			} else {
				blockbuf[48 + 16][NEXTWEST] = 48 +  5;
				blockbuf[48 +  5][NEXTEAST] = 48 + 16;
				blockbuf[48 +  6][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 13][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 14][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 15][NEXTWEST] = NOBLOCK;
			}
			break;
		case 41:
		case 42:
		case 43:
		case 44:
			if(turnout[42][POSITION] == NORMAL) {
				if(turnout[41][POSITION] == NORMAL) {
					blockbuf[48 +  4][NEXTEAST] = 48 +  5;
					blockbuf[48 +  5][NEXTWEST] = 48 +  4;
					blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
				} else {
					blockbuf[48 +  9][NEXTEAST] = 48 +  5;
					blockbuf[48 +  5][NEXTWEST] = 48 +  9;
					blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
				}
			} else {
				if(turnout[43][POSITION] == NORMAL) {
					blockbuf[48 + 10][NEXTEAST] = 48 +  5;
					blockbuf[48 +  5][NEXTWEST] = 48 + 10;
					blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
					blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
				} else {
					if(turnout[44][POSITION] == NORMAL) {
						blockbuf[48 + 11][NEXTEAST] = 48 +  5;
						blockbuf[48 +  5][NEXTWEST] = 48 + 11;
						blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
						blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 12][NEXTEAST] = NOBLOCK;
					} else {
						blockbuf[48 + 12][NEXTEAST] = 48 +  5;
						blockbuf[48 +  5][NEXTWEST] = 48 + 12;
						blockbuf[48 +  4][NEXTEAST] = NOBLOCK;
						blockbuf[48 +  9][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 10][NEXTEAST] = NOBLOCK;
						blockbuf[48 + 11][NEXTEAST] = NOBLOCK;
					}
				}
			}
			break;
		case 45:
			blockbuf[48 +  3][NEXTWEST] = 48 +  2;
			blockbuf[48 +  2][NEXTEAST] = 48 +  3;
			blockbuf[48 +  9][NEXTWEST] = NOBLOCK;
			break;
		case 46:
		case 47:
		case 48:
			if(turnout[46][POSITION] == NORMAL) {
				blockbuf[48 +  4][NEXTWEST] = 48 +  3;
				blockbuf[48 +  3][NEXTEAST] = 48 +  4;
				blockbuf[48 + 10][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 11][NEXTWEST] = NOBLOCK;
				blockbuf[48 + 12][NEXTWEST] = NOBLOCK;
			} else {
				if(turnout[47][POSITION] == NORMAL) {
					blockbuf[48 + 10][NEXTWEST] = 48 +  3;
					blockbuf[48 +  3][NEXTEAST] = 48 + 10;
					blockbuf[48 +  4][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 11][NEXTWEST] = NOBLOCK;
					blockbuf[48 + 12][NEXTWEST] = NOBLOCK;
				} else {
					if(turnout[48][POSITION] == NORMAL) {
						blockbuf[48 + 11][NEXTWEST] = 48 +  3;
						blockbuf[48 +  3][NEXTEAST] = 48 + 11;
						blockbuf[48 +  4][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 10][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 12][NEXTWEST] = NOBLOCK;
					} else {
						blockbuf[48 + 12][NEXTWEST] = 48 +  3;
						blockbuf[48 +  3][NEXTEAST] = 48 + 12;
						blockbuf[48 +  4][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 10][NEXTWEST] = NOBLOCK;
						blockbuf[48 + 11][NEXTWEST] = NOBLOCK;
					}
				}
			}
			break;
		default :
			break;
		}
	}
}


// Checkmanuals is a start-up routine that reports any manual reverse
//  switches being in the manual position to the logfile.
void checkmanuals(void)
{
    int         i;
    char        logstr[80];
    char	*lockmsg, *routemsg;

    for ( i = 1; i <= NumberTurnouts; i++) {
        if (((InBuf[turnout[i][MANUAL_PORT]] >> turnout[i][MANUAL_BITS]) & 1) == 0) {
            turnout[i][LOCK]  = UNLOCKED;    //Put it into array
	    lockmsg = "UnLocked";
        } else {
            turnout[i][LOCK]  = LOCKED;      //Put it into array
	    lockmsg = "Locked";
	}

        if (((InBuf[turnout[i][POSITION_PORT]] >> turnout[i][POSITION_BITS]) & 1) == 1) {
            turnout[i][POSITION] = REVERSE;    //Put it into array
	    routemsg = "Diverging";
        } else {
            turnout[i][POSITION] = NORMAL;    //Put it into array
	    routemsg = "Normal";
	}
	if (options.turnout_debug && (turnout[i][LOCK] == UNLOCKED)) {    //Put it into array
	    sprintf(logstr, "Warning: Turnout %d is %s, %s at startup", i, lockmsg, routemsg);
	    logmessage(logstr);
	}
    }
}


// Throttlebutton() checks the pushbuttons on the side of the walk-around
//  throttles and if appropriate changes turnouts.
void throttlebutton(void)
{
    int         i;
    static BYTE button[8];
    BYTE        temp;

    if(InBuf[43] == SaveBuf[43]) {
        return;
    }

    temp = InBuf[43] >> 1;      // throw away bit zero

    for (i = 1; i <= 6; i++) {
        if ((int)( temp & 0x01) == 1) {
            button[i] = button[i] | 0x01;   // turn on 1 bit
        } else {
            button[i] = button[i] & 0xfe;   // turn off 1 bit
        }

        temp = temp >> 1;                   // adjust input variable
    }
    for (i = 1; i <= NumberBlocks; i++)  {  // look at each block
	int currentcab = blockbuf[i][CURCAB];

        if ((currentcab == NOCAB) || // no cab assigned
	    (!isOccupied(InBuf, i)) ) {       // no detection
            continue;
        }

            // when i's approachblock is found to be a block with a turnout
            // and is unoccupied, the turnout state could be changed
	    //
	    // Issues: Feedback to operator,
	    //         repeated throwing as button is held down long time
	    //         throwing turnout under train
            // To toggle a turnout use:     OutBuf[?] ^= (0x80 or 0x40)
            //       set to normal use:     OutBuf[?] &= (0x7f or 0xbf)
            //      set to reverse use:     OutBuf[?] |= (0x80 or 0x40)
        if ((button[currentcab] & 1) == 1) { // button pushed
            if(blockbuf[i][DIRECTION] == WESTBOUND) {


            } else {    // direction == EASTBOUND
            }
        }
    }
}

