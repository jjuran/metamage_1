/*
	core.cc
	-------
*/

// sha256
#include "sha256/core.hh"
#include "sha256/table.hh"


namespace crypto
{
	
#ifdef __MC68K__
	
	asm
	void sha256_extend_block( u32* block )
	{
		LINK     A6,#0
		
		MOVEA.L  8(A6),A0
		MOVEA.L  A0,A1
		ADDA.L   #64,A1
		
		MOVEQ.L  #47,D0
	loop:
		MOVE.L   (A0)+,(A1)
		
		MOVE.L   (A0),D1
		MOVE.L   D1,D2
		ROR.L    #7,D1
		SWAP     D2
		ROR.L    #2,D2
		EOR.L    D2,D1
		MOVE.L   (A0),D2
		LSR.L    #3,D2
		EOR.L    D2,D1
		
		ADD.L    32(A0),D1
		ADD.L    D1,(A1)
		
		MOVE.L   52(A0),D1
		SWAP     D1
		ROR.L    #1,D1
		MOVE.L   D1,D2
		ROR.L    #2,D2
		EOR.L    D2,D1
		MOVE.L   52(A0),D2
		LSR.L    #8,D2
		LSR.L    #2,D2
		EOR.L    D2,D1
		ADD.L    D1,(A1)+
		
		DBRA     D0,loop
		
		UNLK     A6
		RTS
	}
	
	asm
	void sha256_rounds( sha256_hash& digest, u32* const block )
	{
		LINK     A6,#0
		MOVEM.L  D3-D7/A2-A5,-(SP)
		
		MOVEA.L  8(A6),A0
		MOVEA.L  12(A6),A1
		
		LEA      sha256_table,A2
		
		MOVE.L   (A0)+,D0
		MOVE.L   (A0)+,D1
		MOVE.L   (A0)+,D2
		MOVEA.L  (A0)+,A3  // keep d in A3
		MOVE.L   (A0)+,D4
		MOVEA.L  (A0)+,A5  // keep f in A5
		MOVE.L   (A0)+,D6
		MOVEA.L  (A0)+,A4  // keep h in A4
		
		MOVEQ.L  #63,D7
	loop:
		ROR.L    #6,D4
		MOVE.L   D4,D3
		ROR.L    #5,D4
		EOR.L    D4,D3
		SWAP     D4
		ROL.L    #2,D4
		EOR.L    D4,D3
		ROR.L    #7,D4
		ADDA.L   D3,A4  // h + s1
		
		MOVE.L   A5,D3
		EOR.L    D6,D3
		AND.L    D4,D3
		EOR.L    D6,D3
		ADDA.L   D3,A4  // h + s1 + ch
		
		ADDA.L   (A2)+,A4  // h + s1 + ch + k[ i ]
		ADDA.L   (A1)+,A4  // h + s1 + ch + k[ i ] + w[ i ] == temp1
		
		ADDA.L   A4,A3  // d + temp1
		
		ROR.L    #2,D0
		MOVE.L   D0,D3
		SWAP     D0
		ROL.L    #5,D0
		EOR.L    D0,D3
		SWAP     D0
		ROL.L    #7,D0
		EOR.L    D0,D3
		SWAP     D0
		ROL.L    #6,D0
		
		ADDA.L   D3,A4  // temp1 + s0
		
		MOVE.L   D0,D3  // a
		AND.L    D1,D3  // a & b
		MOVE.L   D2,D5  // c
		AND.L    D0,D5  // a & c
		EOR.L    D5,D3  // (a & b) ^ (a & c)
		MOVE.L   D2,D5  // c
		AND.L    D1,D5  // b & c
		EOR.L    D5,D3  // maj
		
		ADDA.L   D3,A4  // temp1 + temp2
		
		EXG      A4,D6
		EXG      D6,A5
		EXG      A5,D4
		EXG      D4,A3
		EXG      A3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		DBRA     D7,loop
		
		MOVE.L   A3,D3  // restore d
		MOVE.L   A5,D5  // restore f
		MOVE.L   A4,D7  // restore h
		
		// Accumulate the result.
		ADD.L    D7,-(A0)
		ADD.L    D6,-(A0)
		ADD.L    D5,-(A0)
		ADD.L    D4,-(A0)
		ADD.L    D3,-(A0)
		ADD.L    D2,-(A0)
		ADD.L    D1,-(A0)
		ADD.L    D0,-(A0)
		
		MOVEM.L  (SP)+,D3-D7/A2-A5
		UNLK     A6
		RTS
	}
	
#elif defined( __GNUC__ )
	
	void dummy()
	{
	}
	
#endif
	
}  // namespace crypto
