/*
	at_least_68020.hh
	-----------------
*/

#ifndef MACSYS_HAS_ATLEAST68020_HH
#define MACSYS_HAS_ATLEAST68020_HH


namespace mac {
namespace sys {
	
#ifdef __MC68K__
	
	unsigned char CPUFlag : 0x012F;
	
	/*
		We could have conditionalized just the definition of CPUFlag,
		setting it to 2 for non-68K (which is how it's set in Apple's 68K
		emulator), relying on the optimizer to constant-fold it, instead of
		conditionalizing the entire function -- but MWC68K pessimizes its
		side whether it's a comparison or a bitshift, so here we resort to
		writing it in assembly language.  (Using a bitshift is less intuitive
		than `CMPI #2,CPUFlag; SCC D0`, but it's one word smaller.)
	*/
	
	inline
	asm unsigned char has_at_least_68020()
	{
		/*
			Return false for a 68000 or 68010 (0 or 1);
			return true for anything else.
		*/
		
		MOVE.B   CPUFlag,D0
		LSR.B    #1,D0
	}
	
#else
	
	inline
	unsigned char has_at_least_68020()
	{
		return true;
	}
	
#endif
	
}
}

#endif
