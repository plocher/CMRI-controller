
#ifndef KBDIO_H
#define KBDIO_H

/*
 * Header file for keyboard I/O
 */

extern int   getinputnumber(USIC *usic, int x, int y, int digits);
extern char* getinputstring(USIC *usic, int x, int y, int len);

#endif


