/*
	core.68k.cc
	-----------
*/

/*
	See RFC 1321, "The MD5 Message-Digest Algorithm".
	<http://www.faqs.org/rfcs/rfc1321.html>
*/

// md5
#include "md5/core.hh"
#include "md5/table.hh"


namespace crypto
{
	
#ifdef __MC68K__
	
	asm
	void md5_rounds( md5_digest& digest, u32* const block )
	{
		LINK     A6,#0
		MOVEM.L  D3-D7/A2-A4,-(SP)
		
		MOVEA.L  8(A6),A0
		MOVEA.L  12(A6),A1
		
		MOVE.L   (A0)+,D0
		MOVE.L   (A0)+,D1
		MOVE.L   (A0)+,D2
		MOVE.L   (A0)+,D3
		
		LEA      md5_table,A2
		
		MOVEQ.L  #0,D7
		
	round1:
		MOVEQ.L  #7,D6
		
	round1_inner:
		// F
		MOVE.L   D1,D4
		AND.L    D2,D4
		MOVE.L   D1,D5
		NOT.L    D5
		AND.L    D3,D5
		OR.L     D5,D4
		
		ADD.L    D4,D0
		
		ADD.L    (A1,D7.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADDQ.W   #5,D6
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #22,D6
		BLE.S    round1_inner
		CMPI.W   #64,D7
		BLT.S    round1
		
	round2:
		MOVEQ.L  #5,D6
		MOVEA.W  #4,A3
		MOVEA.L  A3,A4
		
	round2_inner:
		
		// G
		MOVE.L   D3,D4
		AND.L    D1,D4
		MOVE.L   D3,D5
		NOT.L    D5
		AND.L    D2,D5
		OR.L     D5,D4
		
		ADD.L    D4,D0
		
		MOVE.L   A4,D4
		ADD.L    D7,D4
		ANDI.L   #0x3F,D4
		
		ADD.L    (A1,D4.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADD.W    A3,D6
		ADDQ.L   #1,A3
		ADDA.W   #16,A4
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #20,D6
		BLE.S    round2_inner
		CMPI.W   #128,D7
		BLT.S    round2
		
		MOVEQ.L  #7,D5
	round3:
		MOVEQ.L  #4,D6
		MOVEA.W  #20,A4
		
	round3_inner:
		
		// H
		MOVE.L   D1,D4
		EOR.L    D2,D4
		EOR.L    D3,D4
		
		ADD.L    D4,D0
		
		MOVE.L   A4,D4
		SUB.L    D7,D4
		ANDI.L   #0x3F,D4
		
		ADD.L    (A1,D4.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADD.W    D5,D6
		EORI.L   #2,D5
		ADDA.W   #16,A4
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #23,D6
		BLE.S    round3_inner
		CMPI.W   #192,D7
		BLT.S    round3
		
		MOVEQ.L  #0,D5
	round4:
		MOVEQ.L  #6,D6
		MOVEA.W  #4,A3
		
	round4_inner:
		
		// I
		MOVE.L   D3,D4
		NOT.L    D4
		OR.L     D1,D4
		EOR.L    D2,D4
		
		ADD.L    D4,D0
		
		MOVE.L   D5,D4
		SUB.L    D7,D4
		ANDI.L   #0x3F,D4
		
		ADD.L    (A1,D4.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADD.W    A3,D6
		ADDQ.L   #1,A3
		EORI.W   #32,D5
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #21,D6
		BLE.S    round4_inner
		CMPI.W   #256,D7
		BLT.S    round4
		
		// Accumulate the result.
		ADD.L    D3,-(A0)
		ADD.L    D2,-(A0)
		ADD.L    D1,-(A0)
		ADD.L    D0,-(A0)
		
		MOVEM.L  (SP)+,D3-D7/A2-A4
		UNLK     A6
		RTS
	}
	
#elif defined( __GNUC__ )
	
	void dummy()
	{
	}
	
#endif
	
}  // namespace crypto
