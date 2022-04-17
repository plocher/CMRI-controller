// @(#)cards.h	1.2 99/08/18 SBHRS

#ifndef _CARDS_H_
#define _CARDS_H_

#include "rr.h"

// const int   NumberInputCards    = 1;
// const int   NumberOutputCards   = 1;
// const int   NumberCardSets      = 1;
// const int   NumberOutputPorts   = NumberOutputCards * 3;
// const int   NumberInputPorts    = NumberInputCards  * 3;


class Cards
{
    private:
	int   ValueOfCard(char x);
	void  InitCardTypes(char *cardlist);

	int   my_NumberInputCards;
	int   my_NumberOutputCards;
	int   my_NumberCardSets;

    public:
              Cards();
              Cards(char *cardlist);

	void  print();
	int   Type(int cardnum);

	int   InputBufferSize()   { return NumberInputPorts()  + 1; }
	int   OutputBufferSize()  { return NumberOutputPorts() + 1; }

	int   NumberCardSets()    { return my_NumberCardSets;        }
	int   NumberInputCards()  { return my_NumberInputCards;      }
	int   NumberOutputCards() { return my_NumberOutputCards;     }
	int   NumberOutputPorts() { return my_NumberOutputCards * 3; }
	int   NumberInputPorts()  { return my_NumberInputCards  * 3; }

	BYTE     CardType[16];    // Card array for interface
};

#endif

