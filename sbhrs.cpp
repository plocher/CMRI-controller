
// @(#)sbhrs.cc	1.3 99/08/27 SBHRS

/*
THIS PROGRAM IS THE PROGRAM FOR OPERATING UNDER
THE CON5TROL OF A DISPATCHER DURING AN OPERATING
SESHION ON THE LAYOUT
*/




#include "rr.h"    // General purpose header file
#include "globals.h"
#include "port.h"  // Port comunications
#include "usic.h"  // USIC comunications
#include "panel.h" // panel definitions

// (((InBuf[10] >> 6) & 0x01) == 0)  panic reset button
// (((InBuf[8]  >> 7) & 0x01) == 0)  panic button 

//**************************************************************************
// The  panel  array contains mask info, port numbers and offsets
//  for each of the ASSIGN/DROP panels.
//      0 --bit mask for assign byte
//      |   1 --mainline port number
//      |   |  2 --number of bits to shift right
//      |   |  |  3 --block 10's port
//      |   |  |  |  4 --block 10's right shift
//      |   |  |  |  |  5 --block 1's port
//      |   |  |  |  |  |  6 --block 1's right shift
//      |   |  |  |  |  |  |  7 --cab port
//      |   |  |  |  |  |  |  |   8 --cab right shift
//      |   |  |  |  |  |  |  |   |   9 --beeper on mask
//      |   |  |  |  |  |  |  |   |   |     10 -- off mask
//      |   |  |  |  |  |  |  |   |   |     |     11 --beeper port
//      |   |  |  |  |  |  |  |   |   |     |     |  12 -- usic card
//      |   |  |  |  |  |  |  |   |   |     |     |  |
//      v   v  v  v  v  v  v  v   v   v     v     v  v
//      0   1  2  3  4  5  6  7   8   9    10    11  12
int panel[7][13] ={
    { 0x00, 0, 0, 0, 0, 0, 0, 0,  0, 0x00, 0x00,  0,  0}, // zero not used
    { 0x01, 1, 6, 4, 0, 5, 4, 8,  4, 0x40, 0xBF,  8,  0}, // panel #1
    { 0x02, 2, 6, 4, 2, 6, 0, 9,  0, 0x80, 0x7F,  8,  0}, // panel #2
    { 0x04, 3, 0, 4, 4, 6, 4, 9,  3, 0x40, 0xBF,  9,  0}, // panel #3
    { 0x08, 3, 2, 4, 6, 7, 0, 10, 0, 0x80, 0x7F,  9,  0}, // panel #4
    { 0x10, 3, 4, 5, 0, 7, 4, 10, 3, 0x40, 0xBF, 10,  0}, // panel #5
    { 0x20, 3, 6, 5, 2, 8, 0, 11, 0, 0x80, 0x7F, 10,  0}  // panel #6
};

// See NumberOfTurnouts for length...
// The  turnout  array contains port numbers and offsets
//  for each of the turnouts on the layout.
//         0 -- turnout control port number
//         |   1 -- number of bits to shift right
//         |   |   2 -- turnout position indicator port
//         |   |   |   3 -- number of bits to shift right
//         |   |   |   |   4 -- manual reverse switch input port
//         |   |   |   |   |   5 -- number of bits to shift right
//         |   |   |   |   |   |  6 -- current turnout position (0 norm, 1 rev)
//         |   |   |   |   |   |  |  7 -- current man rev state (0 norm, 1 rev)
//         |   |   |   |   |   |  |  |  8 -- usic card
//         |   |   |   |   |   |  |  |  |
//         v   v   v   v   v   v  v  v  v
//         0   1   2   3   4   5  6  7  8
int turnout[NumberTurnouts + 1][9] ={
	{  0,  0,  0,  0,  0,  0, 0, 0, 0}, // zero not used
	{  2,  7, 12,  3, 13,  3, 0, 0, 0}, // #1 east drango station
	{ 14,  7, 14,  3, 15,  3, 0, 0, 0}, // #2 east drango station
	{ 14,  6, 14,  2, 15,  2, 0, 0, 0}, // #3 east drango station
	{  2,  6, 12,  2, 13,  2, 0, 0, 0}, // #4 east drango station
        {  1,  7, 12,  1, 13,  1, 0, 0, 0}, // #5 drango jct
	{  1,  6, 12,  0, 13,  0, 0, 0, 0}, // #6 drango jct
	{ 42,  3, 44,  3, 45,  3, 0, 0, 0}, // #7 drango jct
	{ 13,  6, 14,  0, 15,  0, 0, 0, 0}, // #8 drango jct
	{ 13,  7, 14,  1, 15,  1, 0, 0, 0}, // #9 drango jct

	{ 41,  4, 41,  4, 42,  4, 0, 0, 0}, // #10 crossover in block 2
	{ 37,  7, 17,  7, 19,  7, 0, 0, 0}, // #11 los perditos jct unused
	{ 42,  4, 44,  4, 45,  4, 0, 0, 0}, // #12 los perditos jct unused
        { 42,  1, 44,  1, 45,  1, 0, 0, 0}, // #13 los perditos jct unused
	{ 42,  0, 44,  0, 45,  0, 0, 0, 0}, // #14 los perditos jct unused
	{ 42,  2, 44,  2, 45,  2, 0, 0, 0}, // #15 los perditos jct unused
	{ 41,  1, 41,  1, 42,  1, 0, 0, 0}, // #16 unused
	{ 41,  2, 41,  2, 42,  2, 0, 0, 0}, // #17 unused
	{ 41,  3, 41,  3, 42,  3, 0, 0, 0}, // #18 unused
	{ 42,  5, 44,  5, 45,  5, 0, 0, 0}, // #19 unused
	{ 41,  5, 41,  5, 42,  5, 0, 0, 0}, // #20 unused
	{ 41,  6, 41,  6, 42,  6, 0, 0, 0}, // #21 unused
	{ 41,  7, 41,  7, 42,  7, 0, 0, 0}, // #22 unused

	{ 15,  7, 14,  5, 15,  5, 0, 0, 0}, // #23 show  main xover by green valley
					    // norm = blk ?? to ?? and ?? to ??
	{ 41,  0, 41,  0, 42,  0, 0, 0, 0}, // #24 Grenier xover under green valley
					    // norm = blk ?? to ?? and ?? to ??
	{  3,  6, 12,  4, 13,  4, 0, 0, 0}, // #25 show  main xover under mountin
					    // norm = blk ?? to ?? & ?? to ??
	{  3,  7, 12,  5, 13,  5, 0, 0, 0}, // #26 show  main to stageing under mountin
					    // norm = blk 13 to blk 14
	{  4,  6, 12,  6, 13,  6, 0, 0, 0}, // #27 show  main xover by modules
					    // norm = blk 8 to blk 7
	{  4,  7, 12,  7, 13,  7, 0, 0, 0}, // #28 show  main xover by modules
					    // norm = blk 7 to blk 6
	{ 42,  6, 44,  6, 45,  6, 0, 0, 0}, // #29
	{ 42,  7, 44,  7, 45,  7, 0, 0, 0}, // #30
	{ 37,  6, 17,  6, 19,  6, 0, 0, 0}, // #31 Green Valley west end
	{ 15,  6, 14,  4, 15,  4, 0, 0, 0}, // #32 green valley east
	{ 16,  6, 14,  6, 15,  6, 0, 0, 0}, // #33 Staging Turnout # 1
	{ 16,  7, 14,  7, 15,  7, 0, 0, 0}, // #34 Staging Turnout # 2
	{ 30,  6, 16,  0, 18,  0, 0, 0, 0}, // #35 Staging Turnout # 3
	{ 30,  7, 16,  1, 18,  1, 0, 0, 0}, // #36 Staging Turnout # 4
	{ 31,  6, 16,  2, 18,  2, 0, 0, 0}, // #37 Staging Turnout # 5 XX
	{ 31,  7, 16,  3, 18,  3, 0, 0, 0}, // #38 Staging Turnout # 6 XX
	{ 32,  6, 16,  4, 18,  4, 0, 0, 0}, // #39 Staging Turnout # 7 XX
	{ 32,  7, 16,  5, 18,  5, 0, 0, 0}, // #40 Staging Turnout # 8 XX
	{ 33,  6, 16,  6, 18,  6, 0, 0, 0}, // #41 Staging Turnout # 9 XX
	{ 33,  7, 16,  7, 18,  7, 0, 0, 0}, // #42 Staging Turnout # 10 XX
	{ 34,  6, 17,  0, 19,  0, 0, 0, 0}, // #43 Staging Turnout # 11 XX
	{ 34,  7, 17,  1, 19,  1, 0, 0, 0}, // #44 Staging Turnout # 12 XX
	{ 35,  6, 17,  2, 19,  2, 0, 0, 0}, // #45 Staging Turnout # 13
	{ 35,  7, 17,  3, 19,  3, 0, 0, 0}, // #46 Staging Turnout # 14
	{ 36,  6, 17,  4, 19,  4, 0, 0, 0}, // #47 Staging Turnout # 15
	{ 36,  7, 17,  5, 19,  5, 0, 0, 0}  // #48 Staging Turnout # 16
	// 49-59 
};



//*****************************************************************
// Signal heads
//*****************************************************************
    // West signals/West main  91  94  97 
    // East signals/East main 100 103 106
    // East signals/West main 109 112 115
    // West signals/East main 118 121 124
    // -unused-               127
    // Staging		      130

//*****************************************************************
// Block definitions
//*****************************************************************
//The blockbuf array contains port and position info plus next east
// and next west block info for each track block.  Also has the
// block number translation to physical blocks

int blockbuf[NumberBlocks + 1][BBParms] = {
//    0 -- OutBuf pointer
//    |  1 -- number of bits to shift
//    |  |   2 -- next west block
//    |  |   |   3 -- next east block
//    |  |   |   |    4 -- translation to physcial block
//    |  |   |   |    |   5 -- detector buffer number
//    |  |   |   |    |   |  6 -- detector offset
//    |  |   |   |    |   |  |    7 -- west signal base buffer number
//    |  |   |   |    |   |  |    |  8 -- west signal offset
//    |  |   |   |    |   |  |    |  |  9 -- current cab assigned to block
//    |  |   |   |    |   |  |    |  |  |  10 - current direction(1=W, 2=E)
//    |  |   |   |    |   |  |    |  |  |  |    11 - east signal buffer #
//    |  |   |   |    |   |  |    |  |  |  |    |  12 - east signal offset
//    |  |   |   |    |   |  |    |  |  |  |    |  |  13 - usic card
//    |  |   |   |    |   |  |    |  |  |  |    |  |  |  14 - last cab
//    |  |   |   |    |   |  |    |  |  |  |    |  |  |  |
//    0  1   2   3    4   5  6    7  8  9  10   11 12 13 14
//    v  v   v   v    v   v  v    v  v  v  v    v  v  v  v
   {  0, 0,  0,  0,   0,  0, 0,   0, 0, 0, 1,   0, 0, 0, 0}, // Zero not used
   {  1, 0,  2, 16, 101, 20, 0,  91, 0, 0, 1, 109, 0, 0, 0}, // 1  of west main
   {  1, 3,  3,  1, 102, 20, 1,  91, 1, 0, 1, 109, 1, 0, 0}, // 2  of west main
   {  2, 0,  4,  2, 103, 20, 2,  91, 2, 0, 1, 109, 2, 0, 0}, // 3  of west main
   {  2, 3,  5,  3, 104, 20, 3,  91, 3, 0, 1, 109, 3, 0, 0}, // 4  of west main
   {  3, 0,  6,  4, 105, 20, 4,  91, 4, 0, 1, 109, 4, 0, 0}, // 5  of west main
   {  3, 3,  7,  5, 106, 20, 5,  91, 5, 0, 1, 109, 5, 0, 0}, // 6  of west main
   {  4, 0,  8,  6, 107, 20, 6,  91, 6, 0, 1, 109, 6, 0, 0}, // 7  of west main
   {  4, 3,  9,  7, 108, 20, 7,  91, 7, 0, 1, 109, 7, 0, 0}, // 8  of west main
   {  5, 0, 10,  8, 109, 21, 0,  94, 0, 0, 1, 112, 0, 0, 0}, // 9  of west main
   {  5, 3, 11,  9, 110, 21, 1,  94, 1, 0, 1, 112, 1, 0, 0}, // 10 of west main
   {  6, 0, 12, 10, 111, 21, 2,  94, 2, 0, 1, 112, 2, 0, 0}, // 11 of west main
   {  6, 3, 13, 11, 112, 21, 3,  94, 3, 0, 1, 112, 3, 0, 0}, // 12 of west main
   {  7, 0, 14, 12, 113, 21, 4,  94, 4, 0, 1, 112, 4, 0, 0}, // 13 of west main
   {  7, 3, 15, 13, 114, 21, 5,  94, 5, 0, 1, 112, 5, 0, 0}, // 14 of west main
   {  8, 0, 16, 14, 115, 21, 6,  94, 6, 0, 1, 112, 6, 0, 0}, // 15 of west main
   {  8, 3,  1, 15, 116, 21, 7,  94, 7, 0, 1, 112, 7, 0, 0}, // 16 of west main
   {  9, 0, 99, 99, 117, 22, 0,  97, 0, 0, 1, 115, 0, 0, 0}, // 17 of west main
   {  9, 3, 99, 99, 118, 22, 1,  97, 1, 0, 1, 115, 1, 0, 0}, // 18 of west main
   { 10, 0, 99, 99, 119, 22, 2,  97, 2, 0, 1, 115, 2, 0, 0}, // 19 of west main
   {137, 0, 99, 99, 120, 39, 1,  97, 3, 0, 1, 115, 3, 0, 0}, // 20 of west main
   {137, 0, 99, 99, 121, 39, 2,  97, 4, 0, 1, 115, 4, 0, 0}, // 21 of west main
   {137, 0, 99, 99, 122, 39, 3,  97, 5, 0, 1, 115, 5, 0, 0}, // 22 of west main
   {137, 0, 99, 99, 123, 39, 4,  97, 6, 0, 1, 115, 6, 0, 0}, // 23 of west main
   {137, 0, 99, 99, 123, 39, 5,  97, 7, 0, 1, 115, 7, 0, 0}, // 24 of west main
// now east main
//    0  1   2   3    4   5  6    7  8  9  10 11   12
//    v  v   v   v    v   v  v    v  v  v  v  v    v
   { 13, 0, 26, 40, 201, 31, 0, 118, 0, 0, 2, 100, 0, 0, 0}, // 1  of east main 25
   { 13, 3, 27, 25, 202, 31, 1, 118, 1, 0, 2, 100, 1, 0, 0}, // 2  of east main 26
   { 14, 0, 28, 26, 203, 31, 2, 118, 2, 0, 2, 100, 2, 0, 0}, // 3  of east main 27
   { 14, 3, 29, 27, 204, 31, 3, 118, 3, 0, 2, 100, 3, 0, 0}, // 4  of east main 28
   { 15, 0, 30, 28, 205, 31, 4, 118, 4, 0, 2, 100, 4, 0, 0}, // 5  of east main 29
   { 15, 3, 31, 29, 206, 31, 5, 118, 5, 0, 2, 100, 5, 0, 0}, // 6  of east main 30
   { 16, 0, 32, 30, 207, 31, 6, 118, 6, 0, 2, 100, 6, 0, 0}, // 7  of east main 31
   { 16, 3, 33, 31, 208, 31, 7, 118, 7, 0, 2, 100, 7, 0, 0}, // 8  of east main 32
   { 17, 0, 34, 32, 209, 32, 0, 121, 0, 0, 2, 103, 0, 0, 0}, // 9  of east main 33
   { 17, 3, 36, 33, 210, 32, 1, 121, 1, 0, 2, 103, 1, 0, 0}, // 10 of east main 34
   { 18, 0, 99, 99, 211, 32, 2, 121, 2, 0, 2, 103, 2, 0, 0}, // 11 of east main 35
   { 18, 3, 37, 34, 212, 32, 3, 121, 3, 0, 2, 103, 3, 0, 0}, // 12 of east main 36
   { 19, 0, 38, 36, 213, 32, 4, 121, 4, 0, 2, 103, 4, 0, 0}, // 13 of east main 37
   { 19, 3, 39, 37, 214, 32, 5, 121, 5, 0, 2, 103, 5, 0, 0}, // 14 of east main 38
   { 20, 0, 40, 38, 215, 32, 6, 121, 6, 0, 2, 103, 6, 0, 0}, // 15 of east main 39
   { 20, 3, 25, 39, 216, 32, 7, 121, 7, 0, 2, 103, 7, 0, 0}, // 16 of east main 40
   { 21, 0, 99, 99, 217, 33, 0, 124, 0, 0, 2, 106, 0, 0, 0}, // 17 of east main 41
   { 21, 3, 99, 99, 218, 33, 1, 124, 1, 0, 2, 106, 1, 0, 0}, // 18 of east main 42
   { 22, 0, 56, 99, 219, 33, 2, 124, 2, 0, 2, 106, 2, 0, 0}, // 19 of east main 43
   {137, 0, 99, 99, 220, 39, 6, 124, 3, 0, 2, 106, 3, 0, 0}, // 20 of east main 44
   {137, 0, 99, 99, 221, 39, 7, 124, 4, 0, 2, 106, 4, 0, 0}, // 21 of east main 45
   { 40, 0, 99, 99, 222, 40, 0, 124, 5, 0, 2, 106, 5, 0, 0}, // 22 of east main 46
   {137, 0, 99, 99, 223, 40, 1, 124, 6, 0, 2, 106, 6, 0, 0}, // 23 of east main 47
   {137, 0, 99, 99, 223, 40, 2, 124, 7, 0, 2, 106, 7, 0, 0}, // 24 of east main 48
// Staging tracks
//    0  1   2   3    4   5  6    7  8  9  10 11   12
//    v  v   v   v    v   v  v    v  v  v  v  v    v
   { 10, 3, 99, 50, 401, 23, 5, 130, 3, 0, 1, 130, 0, 0, 0}, // 1  of staging 49
   { 11, 0, 49, 51, 402, 23, 6, 130, 3, 0, 1, 130, 0, 0, 0}, // 2  of staging 50
   { 11, 3, 50, 52, 403, 23, 7, 130, 3, 0, 1, 130, 0, 0, 0}, // 3  of staging 51
   { 12, 0, 51, 53, 404, 24, 0, 130, 3, 0, 1, 130, 0, 0, 0}, // 4  of staging 52
   { 12, 3, 52, 54, 405, 24, 1, 130, 3, 0, 1, 130, 0, 0, 0}, // 5  of staging 53
   { 22, 3, 53, 55, 406, 24, 2, 130, 3, 0, 1, 130, 0, 0, 0}, // 6  of staging 54
   { 23, 0, 54, 56, 407, 24, 3, 130, 3, 0, 1, 130, 0, 0, 0}, // 7  of staging 55
   { 23, 3, 55, 99, 408, 24, 4, 130, 3, 0, 1, 130, 0, 0, 0}, // 8  of staging 56
// (blocks 9 & 10 don't exist)
   { 24, 0, 99, 99, 411, 24, 5, 130, 3, 0, 1, 130, 0, 0, 0}, // 11 of staging 57
   { 24, 3, 99, 99, 412, 24, 6, 130, 3, 0, 1, 130, 0, 0, 0}, // 12 of staging 58
   { 27, 0, 99, 99, 413, 24, 7, 130, 3, 0, 1, 130, 0, 0, 0}, // 13 of staging 59
   { 27, 3, 99, 99, 414, 34, 5, 130, 3, 0, 1, 130, 0, 0, 0}, // 14 of staging 60
   { 28, 0, 99, 99, 415, 34, 6, 130, 3, 0, 1, 130, 0, 0, 0}, // 15 of staging 61
   { 28, 3, 99, 99, 416, 34, 7, 130, 3, 0, 1, 130, 0, 0, 0}, // 16 of staging 62
   { 29, 0, 99, 99, 417, 35, 0, 130, 3, 0, 1, 130, 0, 0, 0}, // 17 of staging 63
   { 29, 3, 99, 99, 418, 35, 1, 130, 3, 0, 1, 130, 0, 0, 0}, // 18 of staging 64
// ORANGE Branch Line
//    0  1   2   3    4   5  6    7  8  9  10 11   12
//    v  v   v   v    v   v  v    v  v  v  v  v    v
   { 30, 0, 66, 99, 301, 25, 0,   0, 0, 0, 1,   0, 0, 0, 0}, // 1 of Orange Branch 65
   { 30, 3, 67, 65, 302, 25, 1,   0, 0, 0, 1,   0, 0, 0, 0}, // 2 of Orange Branch 66
   { 31, 0, 69, 66, 303, 25, 2,   0, 0, 0, 1,   0, 0, 0, 0}, // 3 of Orange Branch 67
   { 31, 3, 99, 99, 304, 25, 3,   0, 0, 0, 1,   0, 0, 0, 0}, // 4 of Orange Branch 68
   { 32, 0, 70, 67, 305, 25, 4,   0, 0, 0, 1,   0, 0, 0, 0}, // 5 of Orange Branch 69
   { 32, 3, 99, 68, 306, 25, 5,   0, 0, 0, 1,   0, 0, 0, 0}, // 6 of Orange Branch 70
   { 33, 0, 99, 68, 307, 25, 6,   0, 0, 0, 1,   0, 0, 0, 0}, // 7 of Orange Branch 71
   { 33, 3, 99, 68, 308, 25, 7,   0, 0, 0, 1,   0, 0, 0, 0}, // 8 of Orange Branch 72
};

//*****************************************************************
//The subblockbuf array contains port and position info plus next east
// and next west block info for each track block.  Also has the
// block number translation to physical blocks

int subblockbuf[NumberSubs + 1][6] = {
//     0 -- detector buffer number
//     |  1 -- detector offset
//     |  |  2 -- turnout # in question
//     |  |  |  3 -- block if (2) normal
//     |  |  |  |  4 -- block if (2) reversed
//     |  |  |  |  |  5 -- usic number
//     |  |  |  |  |  |
//     0  1  2  3  4  5 
//     v  v  v  v  v  v
    {  0, 0, 0, 0, 0, 0}, // Zero not used
    { 22, 3, 0, 1,99, 0}, // sub-     01
    { 22, 4, 0, 1,99, 0}, // sub-     02
    { 22, 5, 0,17,99, 0}, // sub-     03
    { 22, 6, 0,17,99, 0}, // sub-     04
    { 22, 7, 0,18,99, 0}, // sub-     05
    { 23, 0, 0,18,99, 0}, // sub-     06
    { 23, 1, 0,19,99, 0}, // sub-     07
    { 23, 2, 0,19,99, 0}, // sub-     08
    { 23, 3, 0,14,99, 0}, // sub-     09
    { 23, 4, 0,14,99, 0}, // sub-     10
    { 34, 4, 0,52,99, 0}, // sub-404E 11
    // { 35, 2, 0,52,99, 0}, // sub-404E 11
    { 35, 3, 0,52,99, 0}, // sub-404W 12
    { 35, 4, 0,54,99, 0}, // sub-406E 13
    { 35, 5, 0,54,99, 0}, // sub-406W 14
    { 35, 6, 0,57,99, 0}, // sub-411E 15
    { 35, 7, 0,57,99, 0}, // sub-411W 16
    { 36, 0, 0,58,99, 0}, // sub-412E 17
    { 36, 1, 0,58,99, 0}, // sub-412W 18
    { 36, 2, 0,59,99, 0}, // sub-413E 19
    { 36, 3, 0,59,99, 0}, // sub-413W 20
    { 36, 4, 0,60,99, 0}, // sub-414E 21
    { 36, 5, 0,60,99, 0}, // sub-414W 22
    { 36, 6, 0,61,99, 0}, // sub-415E 23
    { 36, 7, 0,61,99, 0}, // sub-415W 24
    { 38, 0, 0,62,99, 0}, // sub-416E 25
    { 38, 1, 0,62,99, 0}, // sub-416W 26
    { 38, 2, 0,63,99, 0}, // sub-417E 27
    { 38, 3, 0,63,99, 0}, // sub-417W 28
    { 38, 4, 0,64,99, 0}, // sub-418E 29
    { 38, 5, 0,64,99, 0}, // sub-418W 30
};
// The  throttle  array contains port numbers and offsets
//  for three throttle functions: the revers switch, the pushbutton
//  and the LED
//     0 -- reverse switch port
//     |   1 -- number of bits to shift right
//     |   |   2 -- pushbutton port
//     |   |   |   3 -- number of bits to shift right
//     |   |   |   |   4 -- LED output port
//     |   |   |   |   |   5 -- number of bits to shift right
//     |   |   |   |   |   |  6 -- current cab direction
//     |   |   |   |   |   |  |  7 -- usic number
//     |   |   |   |   |   |  |  |  8 -- Emulated Speed
//     |   |   |   |   |   |  |  |  |
//     0   1   2   3   4   5  6  7  8
//    v   v   v   v   v   v   v  v  v
int throttle[NumberCabs + 1][9] ={
    {  0,  0,  0,  0,  0,  0, 0, 0, 0}, // zero not used
    { 37,  0, 43,  1, 18,  6, 0, 0, 0}, // throttle #1
    { 37,  1, 43,  2, 18,  7, 0, 0, 0}, // throttle #2
    { 37,  2, 43,  3, 19,  6, 0, 0, 0}, // throttle #3
    { 37,  3, 43,  4, 19,  7, 0, 0, 0}, // throttle #4
    { 37,  4, 43,  5, 20,  6, 0, 0, 0}, // throttle #5
    { 37,  5, 43,  6, 20,  7, 0, 0, 0}, // throttle #6
    { 37,  6, 43,  7, 21,  6, 0, 0, 0}  // throttle #7
};

