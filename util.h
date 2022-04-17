// %W% %E% SBHRS

#ifndef _UTIL_H_
#define _UTIL_H_

// General use utility functions

int mask2bit(int mask);
int pin2port(int pin);
int pin2bit(int pin);
int port2pin(int port, int bit);
char *port2name(int port, int bit);
char *binary(int maxbits, unsigned long x, char sep );

#endif

