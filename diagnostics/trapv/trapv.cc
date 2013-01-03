/*
	trapv.cc
	--------
*/


#pragma exceptions off


static void trapv_test()
{
#ifdef __MC68K__
	
	asm
	{
		MOVE #2, CCR  // set V
		
		TRAPV
	}
	
#endif
}

int main()
{
	trapv_test();
	
	return 0;
}
