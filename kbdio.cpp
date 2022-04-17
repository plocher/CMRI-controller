#include "rr.h"    // General purpose header file
#include "globals.h"
#include "kbdio.h"

int getinputnumber(USIC *usic, int x, int y, int digits)
{
	int numdigits = 0;
	int num = 0;
	int c;

        gotoxy(x,y);
        for(;;) {
		RRLOOP(usic);
		while (kbhit()) {
			c = getch();
			if (c >= '0' && c <= '9') {
				num = num * 10 + (c - '0');
				gotoxy(x,y); cprintf("%d", num);
				numdigits++;
			} else if (c == '\n' || c == '\r') {
				return num;
			} else if (c == ' ' ) {
				return num;
			} else {
				return -1;
			}
			if (numdigits == digits) {
				return num;
			}
		}
        }
}

char* getinputstring(USIC *usic, int x, int y, int len)
{
	int done = 0;
	int pos = 0;
	char line[80];
	char *retval;
	int c;

	line[0] = '\0';
        gotoxy(x,y);
        while (!done) {
		RRLOOP(usic);
		while (kbhit()) {
			c = getch();
			if (c == '\n' || c == '\r' || c == '\026') {
				done++;
				break;
			} else if (c == '\b') {
				if (pos > 0) pos--;
			} else if ((c == ' ') ||
				   (c == '-') ||
				   (c == '[') ||
				   (c == ']') ||
				   (c == '/') ||
				   ((c >= '0') && (c <= '9')) ||
				   ((c >= 'A') && (c <= 'Z')) ||
				   ((c >= 'a') && (c <= 'z'))
				  ) {
				line[pos++] = c;
				line[pos] = '\0';
				gotoxy(x,y); cprintf("%s", line);
			}
			if (pos == 79)  { done++; break; }
			if (pos == len) { done++; break; }
		}
        }
	if (pos > 0) {
		retval = new char[pos + 1];
		strcpy(retval, line);
	} else {
		retval="";
	}
	return retval;
}


