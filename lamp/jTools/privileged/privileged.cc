/*
	privileged.cc
	-------------
*/


#pragma exceptions off


#ifdef __MC68K__

static
void privileged_instruction_test()
{
	asm
	{
		ORI      #0,SR
	}
}

#endif

#ifdef __POWERPC__

static
void privileged_instruction_test()
{
	asm
	{
		mfpvr	r3
	}
}

#endif


int main()
{
	privileged_instruction_test();
	
	return 0;
}
