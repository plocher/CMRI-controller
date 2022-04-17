// @(#)cards.cc	1.4 99/08/27 SBHRS

#include <stdlib.h>
#include <stdio.h>

// #include <sys/types.h>
// #include <unistd.h>

#include <string.h>

#include "rr.h"
#include "cards.h"

// The Chubb system can handle a total of 64 cards per USIC/backplane

// HO Layout at SBHRS
static char	*SBHRS_CardList =
	"IIII"  //  0  1  2  3	Input
	"IIII"  //  4  5  6  7	Input
	"IIII"  //  8  9 10 11	Input
	"IIII"  // 12 13 14 15	Input
	"IIOO"  // 16 17	Input
		//       18 19  Output
	"OOOO"  // 20 21 22 23	Output
	"OOOO"  // 24 25 26 27	Output
	"OOOO"  // 28 29 30 31	Output
	"OOOO"  // 32 33 34 35	Output
	"OOOO"  // 36 37 38 39	Output
	"OOOO"  // 40 41 42 43	Output
	"OOOO"  // 44 45 46 47	Output
	"OOOO"  // 48 49 50 51	Output
	"OOOO"  // 52 53 54 55	Output
	"OOOO"  // 56 57 58 59	Output
	"OOOO"  // 60 61 62 63	Output
	"";	// END

int
Cards::ValueOfCard(char x)
{
	switch(x) {
	case 'I': case 'i': my_NumberInputCards++;   return 1;
	case 'O': case 'o': my_NumberOutputCards++;  return 2;
	default:	    return 0;
	}
}
	
void
Cards::InitCardTypes(char *cardlist)
{
	char  *cp = cardlist;
	int CardIndex	= 0;
	int done = 0;
	my_NumberCardSets = 0;

	for (CardIndex = 0; CardIndex < 16; CardIndex++) {
		CardType[CardIndex] = 0;
	}

	for (done=0, CardIndex = 0, cp = cardlist;
	     !done && cp && CardIndex < 16;
	     cp += 4, CardIndex++) {
		int card[4] = {0,0,0,0};

		if (*(cp + 0) == '\0') { // All 4 cells empty
			done=1;
		} else if (*(cp + 1) == '\0') {
			card[0] = ValueOfCard(*(cp + 0)) << 0;
			my_NumberCardSets++;
			done=1;
		} else if (*(cp + 2) == '\0') {
			card[0] = ValueOfCard(*(cp + 0)) << 0;
			card[1] = ValueOfCard(*(cp + 1)) << 2;
			my_NumberCardSets++;
			done=1;
		} else if (*(cp + 3) == '\0') {
			card[0] = ValueOfCard(*(cp + 0)) << 0;
			card[1] = ValueOfCard(*(cp + 1)) << 2;
			card[2] = ValueOfCard(*(cp + 2)) << 4;
			my_NumberCardSets++;
			done=1;
		} else {
			card[0] = ValueOfCard(*(cp + 0)) << 0;
			card[1] = ValueOfCard(*(cp + 1)) << 2;
			card[2] = ValueOfCard(*(cp + 2)) << 4;
			card[3] = ValueOfCard(*(cp + 3)) << 6;
			my_NumberCardSets++;
		}
		CardType[CardIndex] = card[0] | card[1] | card[2] | card[3];
		printf("CardIndex=%d, numbercardsets=%d\n", CardIndex,  my_NumberCardSets);
	}
}

int Cards::Type(int cardnum) {
	int quartet = cardnum / 4;
	int offset  = cardnum % 4;
	int ct;

	if (quartet >= my_NumberCardSets) return 'X';

	ct = CardType[quartet] >> (offset*2) & 0x03;

	if (ct == 1) return 'I';
	else if (ct == 2) return 'O';
	else return 'E';
}

Cards::Cards()
{
	
	my_NumberInputCards  = 0;
	my_NumberOutputCards = 0;
	my_NumberCardSets    = 0;

	InitCardTypes(SBHRS_CardList);
}

Cards::Cards(char *cardlist)
{
	my_NumberInputCards  = 0;
	my_NumberOutputCards = 0;
	my_NumberCardSets    = 0;

	InitCardTypes(cardlist);
}

void
Cards::print()
{
	printf("  Cards: NumberInputCards  %d\n", NumberInputCards());
	printf("  Cards: NumberOutputCards %d\n", NumberOutputCards());
	printf("  Cards: NumberCardSets    %d\n", NumberCardSets());
	printf("  Cards: NumberOutputPorts %d\n", NumberOutputPorts());
	printf("  Cards: NumberInputPorts  %d\n", NumberInputPorts());

	for (int idx = 0;
	     idx < my_NumberInputCards + my_NumberOutputCards;
	     idx++) {
		printf("%c", this->Type(idx));
	}
	printf("\n");
}


