/*
	core.68k.cc
	-----------
*/

// sha1
#include "sha1/core.hh"


namespace crypto
{
	
#ifdef __MC68K__
	
	asm
	void sha1_extend_block( u32* block )
	{
		LINK     A6,#0
		
		MOVEA.L  8(A6),A0
		MOVEA.L  A0,A1
		ADDA.L   #64,A1
		
		MOVEQ.L  #63,D0
	loop:
		MOVE.L   52(A0),D1
		MOVE.L   32(A0),D2
		EOR.L    D2,D1
		MOVE.L   8(A0),D2
		EOR.L    D2,D1
		MOVE.L   (A0)+,D2
		EOR.L    D2,D1
		ROL.L    #1,D1
		MOVE.L   D1,(A1)+
		
		DBRA     D0,loop
		
		UNLK     A6
		RTS
	}
	
	asm
	void sha1_rounds( sha1_digest& digest, u32* const block )
	{
		LINK     A6,#0
		MOVEM.L  D3-D7,-(SP)
		
		MOVEA.L  8(A6),A0
		MOVEA.L  12(A6),A1
		
		MOVE.L   (A0)+,D0
		MOVE.L   (A0)+,D1
		MOVE.L   (A0)+,D2
		MOVE.L   (A0)+,D3
		MOVE.L   (A0)+,D4
		
		MOVEQ.L  #19,D7
	round1:
		MOVE.L   D0,D5
		ROL.L    #5,D5
		ADD.L    D5,D4
		
		// F
		MOVE.L   D1,D5
		AND.L    D2,D5
		MOVE.L   D1,D6
		NOT.L    D6
		AND.L    D3,D6
		OR.L     D6,D5
		ADD.L    D5,D4
		
		ADDI.L   #0x5A827999,D4
		
		ADD.L    (A1)+,D4
		
		ROR.L    #2,D1
		
		EXG      D4,D3
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		DBRA     D7,round1
		
		MOVEQ.L  #19,D7
	round2:
		MOVE.L   D0,D5
		ROL.L    #5,D5
		ADD.L    D5,D4
		
		// G
		MOVE.L   D1,D5
		EOR.L    D2,D5
		EOR.L    D3,D5
		ADD.L    D5,D4
		
		ADDI.L   #0x6ED9EBA1,D4
		
		ADD.L    (A1)+,D4
		
		ROR.L    #2,D1
		
		EXG      D4,D3
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		DBRA     D7,round2
		
		MOVEQ.L  #19,D7
	round3:
		MOVE.L   D0,D5
		ROL.L    #5,D5
		ADD.L    D5,D4
		
		// H
		MOVE.L   D1,D5
		AND.L    D2,D5
		MOVE.L   D1,D6
		AND.L    D3,D6
		OR.L     D6,D5
		MOVE.L   D2,D6
		AND.L    D3,D6
		OR.L     D6,D5
		ADD.L    D5,D4
		
		ADDI.L   #0x8F1BBCDC,D4
		
		ADD.L    (A1)+,D4
		
		ROR.L    #2,D1
		
		EXG      D4,D3
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		DBRA     D7,round3
		
		MOVEQ.L  #19,D7
	round4:
		MOVE.L   D0,D5
		ROL.L    #5,D5
		ADD.L    D5,D4
		
		// I
		MOVE.L   D1,D5
		EOR.L    D2,D5
		EOR.L    D3,D5
		ADD.L    D5,D4
		
		ADDI.L   #0xCA62C1D6,D4
		
		ADD.L    (A1)+,D4
		
		ROR.L    #2,D1
		
		EXG      D4,D3
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		DBRA     D7,round4
		
		// Accumulate the result.
		ADD.L    D4,-(A0)
		ADD.L    D3,-(A0)
		ADD.L    D2,-(A0)
		ADD.L    D1,-(A0)
		ADD.L    D0,-(A0)
		
		MOVEM.L  (SP)+,D3-D7
		UNLK     A6
		RTS
	}
	
#elif defined( __GNUC__ )
	
	void dummy()
	{
	}
	
#endif
	
}  // namespace crypto
