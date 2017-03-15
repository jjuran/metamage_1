/*
	Gestalt.cc
	----------
*/

#include "Gestalt.hh"


enum
{
	gestaltMachineIcon            = 'micn',
	gestaltUserVisibleMachineName = 'mnam',
};

Gestalt_ProcPtr old_Gestalt;

static
short Gestalt_payload( unsigned long  selector : __D0,
                       long*          response : __A1 ) : __D0
{
	switch ( selector )
	{
		case gestaltMachineIcon:
			*response = 42;
			break;
		
		case gestaltUserVisibleMachineName:
			*(unsigned char**) response = "\p" "Advanced Mac Substitute";
			break;
		
		default:
			return -1;  // Any non-zero value will do here.
	}
	
	return 0;
}

asm
pascal short Gestalt_patch( unsigned long selector : __D0 ) : __D0
{
	MOVE.L   A1,-(SP)         // save A1, the 'response' address
	
	JSR      Gestalt_payload
	
	MOVEA.L  (SP)+,A1         // restore A1
	
	TST.W    D0
	BEQ.S    found
	
	// Not found -- try the native implementation.
	MOVEA.L  old_Gestalt,A0
	JMP      (A0)
	
found:
	// Found -- move the result to A0, where the glue code expects to find it.
	MOVEA.L  (A1),A0
	
	RTS
}
