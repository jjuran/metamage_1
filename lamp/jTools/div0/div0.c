/*
	div0.c
	------
	
	Copyright 2009, Joshua Juran
*/


static int zero = 0;

static void division_by_zero_test()
{
	int q = 1 / zero;
}


int main()
{
	division_by_zero_test();
	
	return 0;
}
