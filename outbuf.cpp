// @(#)outbuf.cc	1.1 98/12/22 SBHRS

// InitOutBuf
// Setup the initial values in the output buffer

#include "rr.h"

void InitOutBuf(BYTE outbuf[])
{

    outbuf[1] = 63;     //card 18A - bits 0-2 b cab assigned to block 1 = 0
                        //card 18A - bits 3-5 b cab assigned to block 2 = 0
                        //card 18A - bit 6 = b turnout #1 = NORMAL = 0
                        //card 18A - bit 7 = b turnout #2 = NORMAL = 0
    outbuf[2] = 63;     //card 18B - bits 0-2 b cab assigned to block 3 = 0
                        //card 18B - bits 3-5 b cab assigned to block 4 = 0
                        //card 18B - bit 6 = b turnout #3 = NORMAL = 0
                        //card 18B - bit 7 = b turnout #4 = NORMAL = 0
    outbuf[3] = 63;     //card 18C - bits 0-2 b cab assigned to block 5 = 0
                        //card 18C - bits 3-5 b cab assigned to block 6 = 0
                        //card 18C - bit 6 = b turnout #5 = NORMAL = 0
                        //card 18C - bit 7 = b turnout #6 = NORMAL = 0

    outbuf[4] = 63;     //card 19A - bits 0-2 b cab assigned to block 7 = 0
                        //card 19A - bits 3-5 b cab assigned to block 8 = 0
                        //card 19A - bit 6 = b turnout #7 = NORMAL = 0
                        //card 19A - bit 7 = b turnout #8 = NORMAL = 0
    outbuf[5] = 63;     //card 19B - bits 0-2 b cab assigned to block 9 = 0
                        //card 19B - bits 3-5 b cab assigned to block 10 = 0
                        //card 19B - bit 6 = b turnout #9 = NORMAL = 0
                        //card 19B - bit 7 = b turnout #10 = NORMAL = 0
    outbuf[6] = 63;     //card 19C - bits 0-2 b cab assigned to block 11 = 0
                        //card 19C - bits 3-5 b cab assigned to block 12 = 0
                        //card 19C - bit 6 spare
                        //card 19C - bit 7 spare

    outbuf[7] = 255;    //card 20A - bits 0-2 b cab assigned to block 13 = 0
                        //card 20A - bits 3-5 b cab assigned to block 14 = 0
                        //card 20A - bit 6 spare
                        //card 20A - bit 7 spare
    outbuf[8] = 63; //card 20B - bits 0-2 b cab assigned to block 15 = 0
                        //card 20B - bits 3-5 b cab assigned to block 16 = 0
                        //card 20B - bit 6 B/Cab annunciator #1 = 0
                        //card 20B - bit 7 B/Cab annunciator #2 = 0
    outbuf[9] = 63; //card 20C - bits 0-2 b cab assigned to block 17 = 0
                        //card 20C - bits 3-5 b cab assigned to block 18 = 0
                        //card 20C - bit 6 B/Cab annunciator #3 = 0
                        //card 20C - bit 7 B/Cab annunciator #4 = 0

    outbuf[10] = 63;    //card 21A - bits 0-2 b cab assigned to block 19 = 0
                        //card 21A - bits 3-5 b cab assigned to block 20 = 0
                        //card 21A - bit 6 B/Cab annunciator #5 = 0
                        //card 21A - bit 7 B/Cab annunciator #6 = 0
    outbuf[11] = 255;   //card 21B - bits 0-2 Reserved for b block expansion
                        //card 21B - bits 3-5     "     "  "   "       "
                        //card 21B - bit 6 spare
                        //card 21B - bit 7 Cycle LED
    outbuf[12] = 63;    //card 21C - bits 0-2 Reserved for b block expansion
                        //card 21C - bits 3-5     "     "  "   "       "
                        //card 21C - bit 6 East b yard approach
                        //card 21C - bit 7 West b yard approach

    outbuf[13] = 63;    //card 22A - bits 0-2 y cab assigned to block 1 = 0
                        //card 22A - bits 3-5 y cab assigned to block 2 = 0
                        //card 22A - bit 6 y turnout #1 = NORMAL = 0
                        //card 22A - bit 7 y turnout #2 = NORMAL = 0
    outbuf[14] = 63;    //card 22B - bits 0-2 y cab assigned to block 3 = 0
                        //card 22B - bits 3-5 y cab assigned to block 4 = 0
                        //card 22B - bit 6 y turnout #3 = NORMAL = 0
                        //card 22B - bit 7 y turnout #4 = NORMAL = 0
    outbuf[15] = 63;    //card 22C - bits 0-2 y cab assigned to block 5 = 0
                        //card 22C - bits 3-5 y cab assigned to block 6 = 0
                        //card 22C - bit 6 y turnout #5 = NORMAL = 0
                        //card 22C - bit 7 y turnout #6 = NORMAL = 0

    outbuf[16] = 255;    //card 23A - bits 0-2 y cab assigned to block 7 = 0
                        //card 23A - bits 3-5 y cab assigned to block 8 = 0
                        //card 23A - bit 6 y turnout #7 = NORMAL = 0
                        //card 23A - bit 7 y turnout #8 = NORMAL = 0
    outbuf[17] = 63;    //card 23B - bits 0-2 y cab assigned to block 9 = 0
                        //card 23B - bits 3-5 y cab assigned to block 10 = 0
                        //card 23B - bit 6 y turnout #9 = NORMAL = 0
                        //card 23B - bit 7 y turnout #10 = NORMAL = 0
    outbuf[18] = 63;    //card 23C - bits 0-2 y cab assigned to block 11 = 0
                        //card 23C - bits 3-5 y cab assigned to block 12 = 0
                        //card 23C - bit 6 spare
                        //card 23C - bit 7 spare

    outbuf[19] = 63;    //card 24A - bits 0-2 y cab assigned to block 13 = 0
                        //card 24A - bits 3-5 y cab assigned to block 14 = 0
                        //card 24A - bit 6 spare
                        //card 24A - bit 7 spare
    outbuf[20] = 63;    //card 24B - bits 0-2 y cab assigned to block 15 = 0
                        //card 24B - bits 3-5 y cab assigned to block 16 = 0
                        //card 24B - bit 6 spare
                        //card 24B - bit 7 spare
    outbuf[21] = 63;    //card 24C - bits 0-2 y cab assigned to block 17 = 0
                        //card 24C - bits 3-5 y cab assigned to block 18 = 0
                        //card 24C - bit 6 spare
                        //card 24C - bit 7 spare

    outbuf[22] = 63;    //card 25A - bits 0-2 y cab assigned to block 19 = 0
                        //card 25A - bits 3-5 y cab assigned to block 20 = 0
                        //card 25A - bit 6 spare
                        //card 25A - bit 7 spare
    outbuf[23] = 63;    //card 25B - bits 0-2 Reserved for y block expansion
                        //card 25B - bits 3-5     "     "  "   "       "
                        //card 25B - bit 6 spare
                        //card 25B - bit 7 spare
    outbuf[24] = 63;    //card 25C - bits 0-2 Staging block #411
                        //card 25C - bits 3-5 Staging block #412
                        //card 25C - bit 6 East y yard approach
                        //card 25C - bit 7 West y yard approach

    outbuf[25] = 0;     //card 26A - bits 0-3 Blue throttle #1
                        //card 26A - bits 4-7 Blue throttle #2
    outbuf[26] = 0;     //card 26B - bits 0-3 Blue throttle #3
                        //card 26B - bits 4-7 Blue throttle #4
    outbuf[27] = 63;    //card 26C - bits 0-2 Staging block #413
                        //card 26C - bits 3-5 Staging block #414
                        //card 26C - bit 6 spare
                        //card 26C - bit 7 spare

    outbuf[28] = 63;    //card 27A - bits 0-2 Staging block #415
                        //card 27A - bits 3-5 Staging block #416
                        //card 27A - bit 6 spare
                        //card 27A - bit 7 spare
    outbuf[29] = 63;    //card 27B - bits 0-2 Staging block #417
                        //card 27B - bits 3-5 Staging block #418
                        //card 27B - bit 6 spare
                        //card 27B - bit 7 spare
    outbuf[30] = 255;    //card 27C - bits 0-3 w cab assigned to block #1
                        //card 27C - bits 4-7 w cab assigned to block #2


    outbuf[31] = 255;    //card 28A - bits 0-3 w cab assigned to block #3
                        //card 28A - bits 4-7 w cab assigned to block #4
    outbuf[32] = 255;    //card 28B - bits 0-3 w cab assigned to block #5
                        //card 28B - bits 4-7 w cab assigned to block #6
    outbuf[33] = 63;    //card 28C - bits 0-3 w cab assigned to block #7
                        //card 28C - bits 4-7 w cab assigned to block #8

    outbuf[34] = 63;    //card 29A - bits 0-3 w cab assigned to block #9
                        //card 29A - bits 4-7 w cab assigned to block #10
    outbuf[35] = 63;    //card 29B - bits 0-3 w cab assigned to block #11
                        //card 29B - bits 4-7 w cab assigned to block #12
    outbuf[36] = 63;    //card 29C - bits 0-3 w cab assigned to block #13
                        //card 29C - bits 4-7 w cab assigned to block #14

    outbuf[37] = 63;    //card 30A - bits 0-3 w cab assigned to block #15
                        //card 30A - bits 4-7 w cab assigned to block #16
    outbuf[38] = 63;    //card 30B - bits 0-2 w cab assigned to block #17
                        //card 30B - bits 3-5 w cab assigned to block #18
                        //card 30B - bit  6   staging turnouts manual control
    outbuf[39] = 63;    //card 30C - bits 0-3 w cab assigned to block #19
                        //card 30C - bits 4-7 w cab assigned to block #20

    outbuf[40] = 63;    //card 31A - bits 0-3 w cab assigned to block #21
                        //card 31A - bits 4-7 w cab assigned to block #22
    outbuf[41] =  0;    //card 31B - bits 0-3 w cab assigned to block #23
                        //card 31B - bits 4-7 w cab assigned to block #24
    outbuf[42] =  0;    //card 31C - bits 0-3 w cab assigned to block #25
                        //card 31C - bits 4-7 w cab assigned to block #26

        // Cards 32 - 41 are reserved for white expansion

    outbuf[43] = 0;     //card 32A
    outbuf[44] = 0;     //card 32B
    outbuf[45] = 0;     //card 32C

    outbuf[46] = 0;     //card 33A
    outbuf[47] = 0;     //card 33B
    outbuf[48] = 0;     //card 33C

    outbuf[49] = 0;     //card 34A
    outbuf[50] = 0;     //card 34B
    outbuf[51] = 0;     //card 34C

    outbuf[52] = 0;     //card 35A
    outbuf[53] = 0;     //card 35B
    outbuf[54] = 0;     //card 35C

    outbuf[55] = 0;     //card 36A
    outbuf[56] = 0;     //card 36B
    outbuf[57] = 0;     //card 36C

    outbuf[58] = 0;     //card 37A
    outbuf[59] = 0;     //card 37B
    outbuf[60] = 0;     //card 37C

    outbuf[61] = 0;     //card 38A
    outbuf[62] = 0;     //card 38B
    outbuf[63] = 0;     //card 38C

    outbuf[64] = 0;     //card 39A
    outbuf[65] = 0;     //card 39B
    outbuf[66] = 0;     //card 39C

    outbuf[67] = 0;     //card 40A
    outbuf[68] = 0;     //card 40B
    outbuf[69] = 0;     //card 40C

    outbuf[70] = 0;     //card 41A
    outbuf[71] = 0;     //card 41B
    outbuf[72] = 0;     //card 41C

    outbuf[73] = 255;   //card 42A bit 0 - Blue Red aspect #1
                        //card 42A bit 1 - Blue Red aspect #2
                        //card 42A bit 2 - Blue Red aspect #3
                        //card 42A bit 3 - Blue Red aspect #4
                        //card 42A bit 4 - Blue Red aspect #5
                        //card 42A bit 5 - Blue Red aspect #6
                        //card 42A bit 6 - Blue Red aspect #7
                        //card 42A bit 7 - Blue Red aspect #8
    outbuf[74] = 0;     //card 42B bit 0 - Yellow Red aspect #1
                        //card 42A bit 1 - Yellow Red aspect #2
                        //card 42A bit 2 - Yellow Red aspect #3
                        //card 42A bit 3 - Yellow Red aspect #4
                        //card 42A bit 4 - Yellow Red aspect #5
                        //card 42A bit 5 - Yellow Red aspect #6
                        //card 42A bit 6 - Yellow Red aspect #7
                        //card 42A bit 7 - Yellow Red aspect #8
    outbuf[75] = 0;     //card 42C bit 0 - Green Red aspect #1
                        //card 42A bit 1 - Green Red aspect #2
                        //card 42A bit 2 - Green Red aspect #3
                        //card 42A bit 3 - Green Red aspect #4
                        //card 42A bit 4 - Green Red aspect #5
                        //card 42A bit 5 - Green Red aspect #6
                        //card 42A bit 6 - Green Red aspect #7
                        //card 42A bit 7 - Green Red aspect #8

    outbuf[76] = 255;   //card 43A bit 0 - Blue Red aspect #9
                        //card 43A bit 1 - Blue Red aspect #10
                        //card 43A bit 2 - Blue Red aspect #11
                        //card 43A bit 3 - Blue Red aspect #12
                        //card 43A bit 4 - Blue Red aspect #13
                        //card 43A bit 5 - Blue Red aspect #14
                        //card 43A bit 6 - Blue Red aspect #15
                        //card 43A bit 7 - Blue Red aspect #16
    outbuf[77] = 0;     //card 43B bit 0 - Yellow Red aspect #9
                        //card 43A bit 1 - Yellow Red aspect #10
                        //card 43A bit 2 - Yellow Red aspect #11
                        //card 43A bit 3 - Yellow Red aspect #12
                        //card 43A bit 4 - Yellow Red aspect #13
                        //card 43A bit 5 - Yellow Red aspect #14
                        //card 43A bit 6 - Yellow Red aspect #15
                        //card 43A bit 7 - Yellow Red aspect #16
    outbuf[78] = 0;     //card 43C bit 0 - Green Red aspect #9
                        //card 43A bit 1 - Green Red aspect #10
                        //card 43A bit 2 - Green Red aspect #11
                        //card 43A bit 3 - Green Red aspect #12
                        //card 43A bit 4 - Green Red aspect #13
                        //card 43A bit 5 - Green Red aspect #14
                        //card 43A bit 6 - Green Red aspect #15
                        //card 43A bit 7 - Green Red aspect #16

    outbuf[79] = 255;   //card 44A bit 0 - Blue Red aspect #17
                        //card 44A bit 1 - Blue Red aspect #18
                        //card 44A bit 2 - Blue Red aspect #19
                        //card 44A bit 3 - Blue Red aspect #20
                        //card 44A bit 4 - Blue Red aspect expansion
                        //card 44A bit 5 - Blue Red aspect expansion
                        //card 44A bit 6 - Blue Red aspect expansion
                        //card 44A bit 7 - Blue Red aspect expansion
    outbuf[80] = 0;     //card 44B bit 0 - Yellow Red aspect #17
                        //card 44A bit 1 - Yellow Red aspect #18
                        //card 44A bit 2 - Yellow Red aspect #19
                        //card 44A bit 3 - Yellow Red aspect #20
                        //card 44A bit 4 - Yellow Red aspect expansion
                        //card 44A bit 5 - Yellow Red aspect expansion
                        //card 44A bit 6 - Yellow Red aspect expansion
                        //card 44A bit 7 - Yellow Red aspect expansion
    outbuf[81] = 0;     //card 44C bit 0 - Green Red aspect #17
                        //card 44A bit 1 - Green Red aspect #18
                        //card 44A bit 2 - Green Red aspect #19
                        //card 44A bit 3 - Green Red aspect #20
                        //card 44A bit 4 - Green Red aspect expansion
                        //card 44A bit 5 - Green Red aspect expansion
                        //card 44A bit 6 - Green Red aspect expansion
                        //card 44A bit 7 - Green Red aspect expansion

    outbuf[82] = 0;     //card 45A
    outbuf[83] = 0;     //card 45B
    outbuf[84] = 0;     //card 45C

    outbuf[85] = 0;     //card 46A
    outbuf[86] = 0;     //card 46B
    outbuf[87] = 0;     //card 46C

    outbuf[88] = 0;     //card 47A
    outbuf[89] = 0;     //card 47B
    outbuf[90] = 0;     //card 47C

    outbuf[91] = 255;   //card 48A
    outbuf[92] = 0;     //card 48B
    outbuf[93] = 0;     //card 48C

    outbuf[94] = 255;   //card 49A
    outbuf[95] = 0;     //card 49B
    outbuf[96] = 0;     //card 49C

    outbuf[97] = 255;   //card 50A
    outbuf[98] = 0;     //card 50B
    outbuf[99] = 0;     //card 50C

    outbuf[100] = 255;  //card 51A
    outbuf[101] = 0;    //card 51B
    outbuf[102] = 0;    //card 51C

    outbuf[103] = 255;  //card 52A
    outbuf[104] = 0;    //card 52B
    outbuf[105] = 0;    //card 52C

    outbuf[106] = 255;  //card 53A
    outbuf[107] = 0;    //card 53B
    outbuf[108] = 0;    //card 53C

    outbuf[109] = 255;  //card 54A
    outbuf[110] = 0;    //card 54B
    outbuf[111] = 0;    //card 54C

    outbuf[112] = 255;  //card 55A
    outbuf[113] = 0;    //card 55B
    outbuf[114] = 0;    //card 55C

    outbuf[115] = 255;  //card 56A
    outbuf[116] = 0;    //card 56B
    outbuf[117] = 0;    //card 56C

    outbuf[118] = 255;  //card 57A
    outbuf[119] = 0;    //card 57B
    outbuf[120] = 0;    //card 57C

    outbuf[121] = 255;  //card 58A
    outbuf[122] = 0;    //card 58B
    outbuf[123] = 0;    //card 58C

    outbuf[124] = 255;  //card 59A
    outbuf[125] = 0;    //card 59B
    outbuf[126] = 0;    //card 59C

    outbuf[127] = 0;    //card 60A
    outbuf[128] = 0;    //card 60B
    outbuf[129] = 0;    //card 60C

    outbuf[130] = 0;    //card 61A
    outbuf[131] = 0;    //card 61B
    outbuf[132] = 0;    //card 61C

    outbuf[133] = 0;    //card 62A
    outbuf[134] = 0;    //card 62B
    outbuf[135] = 0;    //card 62C

    outbuf[136] = 0;    //card 63A
    outbuf[137] = 0;    //card 63B
    outbuf[138] = 0;    //card 63C

}

