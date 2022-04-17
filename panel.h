// @(#)panel.h	1.1 98/12/22 SBHRS

#ifndef _PANEL_H_
#define _PANEL_H_

#define     PANEL_BUTTON		0
#define     PANEL_MAINLINE		1
#define     PANEL_MAINSHIFT		2
#define     PANEL_10PORT		3
#define     PANEL_10SHIFT		4
#define     PANEL_1PORT			5
#define     PANEL_1SHIFT		6
#define     PANEL_CABPORT		7
#define     PANEL_CABSHIFT		8
#define     PANEL_BEEPER_ON		9
#define     PANEL_BEEPER_OFF		10
#define     PANEL_BEEPER		11

#define     PANEL_ASSIGN                1
#define     PANEL_DROP                  2
#define     PANEL_MAINLINEMASK		0x03
#define     PANEL_10MASK		0x03
#define     PANEL_1MASK			0x0F
#define     PANEL_CABMASK		0x07

#define     PANEL_GET_MAIN(Buffer, panel) \
		(((Buffer)[(panel)[PANEL_MAINLINE]] >> (panel)[PANEL_MAINSHIFT]) & PANEL_MAINLINEMASK)

#define     PANEL_GET_BLOCK(Buffer, panel) \
		(((((Buffer)[(panel)[PANEL_10PORT]] >> (panel)[PANEL_10SHIFT]) & PANEL_10MASK) * 10) + \
		  (((Buffer)[(panel)[PANEL_1PORT]]  >> (panel)[PANEL_1SHIFT])  & PANEL_1MASK))

#define     PANEL_GET_CAB(Buffer, panel) \
		(((Buffer)[(panel)[PANEL_CABPORT]] >> (panel)[PANEL_CABSHIFT]) & PANEL_CABMASK)

#define     PANEL_SET_BEEP(Buffer, panel) { \
		(Buffer)[(panel)[PANEL_BEEPER]] = (Buffer)[(panel)[PANEL_BEEPER]] | (panel)[PANEL_BEEPER_ON]; \
	    }
#define     PANEL_RESET_BEEP(Buffer, panel) { \
		(Buffer)[(panel)[PANEL_BEEPER]] = (Buffer)[(panel)[PANEL_BEEPER]] & (panel)[PANEL_BEEPER_OFF]; \
	    }

#endif


