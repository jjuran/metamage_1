/*
	Packages.cc
	-----------
*/

#include "Packages.hh"

#ifndef __TRAPS__
#include <Traps.h>
#endif


char** AppPacks : 0x0AB8;

enum
{
	n_packs = 8,
};

pascal void InitPack_patch( short packID )
{
}

pascal void InitAllPacks_patch()
{
}

/*
	Stack on entry to PackN_patch():
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	
	PC = PACK_N_patch
*/

/*
	Stack on entry to call_PACK():
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	| 0x0000000x     |  4-byte PACK id
	+----------------+
	
	PC = call_PACK
*/

/*
	Stack after first MOVEM:
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	| 0x0000000x     |  4-byte PACK id
	+----------------+
	| D0-D2 / A0-A2  |  24 bytes
	+----------------+
*/

/*
	Stack before _GetResource:
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	| 0x0000000x     |  4-byte PACK id
	+----------------+
	| D0-D2 / A0-A2  |  24 bytes
	+----------------+
	| uninitialized  |  4 bytes
	+----------------+
	| 'PACK'         |  4 bytes
	+----------------+
	| 0x000x         |  2-byte PACK id
	+----------------+
*/

/*
	Stack after _GetResource:
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	| 0x0000000x     |  4-byte PACK id
	+----------------+
	| D0-D2 / A0-A2  |  24 bytes
	+----------------+
	| handle or NULL |  4 bytes
	+----------------+
*/

/*
	Stack at invoke:
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	| 0x0000000x     |  4-byte PACK id
	+----------------+
	| D0-D2 / A0-A2  |  24 bytes
	+----------------+
*/

/*
	Stack before last MOVEM:
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	| PACK pointer   |  4 bytes
	+----------------+
	| D0-D2 / A0-A2  |  24 bytes
	+----------------+
*/

/*
	Stack after last MOVEM:
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	| PACK pointer   |  4 bytes
	+----------------+
*/

/*
	Stack after RTS:
	
	:                :
	+----------------+
	| arguments ...  |
	+----------------+
	| return address |  4 bytes
	+----------------+
	
	PC = package code
*/

static
asm void call_PACK( long i )
{
	MOVEM.L  D0-D2/A0-A2,-(SP)  // 24 bytes
	
	MOVE.W   26(SP),D0          // PACK id
	CHK.W    #(n_packs - 1),D0
	LSL.W    #2,D0
	LEA      AppPacks,A0
	LEA      (A0,D0.W),A2       // Address storing the PACK handle
	
	TST.L    (A2)
	BNE.S    invoke
	
	SUBQ.L   #4,SP
	MOVE.L   #'PACK',-(SP)
	MOVE.W   34(SP),-(SP)
	_GetResource
	MOVE.L   (SP)+,(A2)
	
	// Check for failure
	BNE.S    invoke
	
	MOVE.W   26(SP),D0
	ADDI.W   #17,D0
	_SysError
	
invoke:
	MOVEA.L  (A2),A0  // PACK handle
	MOVEA.L  (A0),A0  // PACK pointer
	
	MOVE.L   A0,24(SP)
	MOVEM.L  (SP)+,D0-D2/A0-A2
	RTS
}

asm void Pack0_patch()
{
	CLR.L    -(SP)
	JMP      call_PACK
}

asm void Pack1_patch()
{
	PEA      0x0001
	JMP      call_PACK
}

asm void Pack2_patch()
{
	PEA      0x0002
	JMP      call_PACK
}

asm void Pack3_patch()
{
	PEA      0x0003
	JMP      call_PACK
}

asm void Pack4_patch()
{
	PEA      0x0004
	JMP      call_PACK
}

asm void Pack5_patch()
{
	PEA      0x0005
	JMP      call_PACK
}

asm void Pack6_patch()
{
	PEA      0x0006
	JMP      call_PACK
}

asm void Pack7_patch()
{
	PEA      0x0007
	JMP      call_PACK
}
