/*
	illegal.c
	---------
*/


#ifdef __MC68K__

static
void illegal_instruction_test()
{
	asm
	{
		ILLEGAL  // guaranteed to be an illegal instruction
	}
}

#endif

#ifdef __POWERPC__

static
asm void the_real_illegal_instruction_test()
{
	/*
		A zero word is guaranteed to be an illegal instruction,
		but there doesn't seem to be a way to trick MWCPPC into
		assembling one.  But every function is followed by a
		zero word (I think), so just falling off the end of the
		function would work, except that MWCPPC helpfully adds a
		blr instruction in case we forgot.  So we'll jump over it.
		No big deal.
	*/
	
	b		end  // skip over the blr
	
	blr  // this suppresses the assembler-inserted blr
	
end:
	// we're pointing to the zero word following the blr.  Yay!
}

static
asm void illegal_instruction_test()
{
	/*
		This is really here for the traceback table.
		Since we immediately branch to another function,
		we get the same results as if we had called that
		function in the first place.  However, since the
		address of the illegal instruction follows the blr,
		the backtrace has already skipped the traceback table,
		and stops on the next one, which is here.
	*/
	
	b	the_real_illegal_instruction_test
}

#endif


int main()
{
	illegal_instruction_test();
	
	return 0;
}
