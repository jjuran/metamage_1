/*
	chk.c
	-----
*/


static
void chk_test()
{
#ifdef __MC68K__
	
	asm
	{
		MOVEQ    #-1,D0
		
		CHK.W    #0,D0
	}
	
#endif
}

int main()
{
	chk_test();
	
	return 0;
}
