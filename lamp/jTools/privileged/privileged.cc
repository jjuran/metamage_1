/*	=============
 *	privileged.cc
 *	=============
 */


#pragma exceptions off


#ifdef __MC68K__

static void PrivilegedInstructionTest()
{
	asm
	{
		machine 68020
		
		MOVEC	VBR,A0
	}
}

#endif

#ifdef __POWERPC__

static void PrivilegedInstructionTest()
{
	asm
	{
		mfpvr	r3
	}
}

#endif


int main()
{
	PrivilegedInstructionTest();
	
	return 0;
}

