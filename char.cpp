#include <stdio.h>

p(int c) { printf("%3d: %c ", c, (char)c); }

main()
{
	int x;
	for (x = 129; x <=255;)
	// for (x = 1; x <=128;)
	{
		p(x++);
		p(x++);
		p(x++);
		p(x++);
		p(x++);
		p(x++);
		p(x++);
		p(x++);
		printf("\n");
	}

}

