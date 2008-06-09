/*	==========
 *	illegal.cc
 *	==========
 */


#pragma exceptions off


#ifdef __MC68K__

static void IllegalInstructionTest()
{
	asm
	{
		ILLEGAL  // guaranteed to be an illegal instruction
	}
}

inline void TheRealIllegalInstructionTest()
{
}

#endif

#ifdef __POWERPC__

static asm void TheRealIllegalInstructionTest()
{
	// a zero word is guaranteed to be an illegal instruction, but there doesn't
	// seem to be a way to trick MWCPPC into assembling one.
	// But every function is followed by a zero word (I think), so just falling
	// off the end of the function would work, except that MWCPPC helpfully adds
	// a blr instruction in case we forgot.
	// So we'll jump over it.  No big deal.
	
	b		end  // skip over the blr
	
	blr  // this suppresses the assembler-inserted blr
	
end:
	// we're pointing to the zero word following the blr.  Yay!
}

static void IllegalInstructionTest()
{
	// This is only here for the traceback table.
}

#endif


int main()
{
	IllegalInstructionTest();
	TheRealIllegalInstructionTest();
	
	return 0;
}

