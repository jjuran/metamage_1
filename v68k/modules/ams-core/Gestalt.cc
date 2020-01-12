/*
	Gestalt.cc
	----------
*/

#include "Gestalt.hh"

// ams-common
#include "reactor-gestalt.hh"

// ams-core
#include "reactor-core.hh"


enum
{
	gestaltMachineIcon            = 'micn',
	gestaltUserVisibleMachineName = 'mnam',
	gestaltQuickdrawVersion       = 'qd  ',
	gestaltSystemVersion          = 'sysv',
};

Gestalt_ProcPtr old_Gestalt;

static
long Gestalt_payload( unsigned long  selector : __D0,
                      long*          response : __A1 )
{
	switch ( selector )
	{
		case gestaltQuickdrawVersion:
		case gestaltSystemVersion:
			*response = 0;
			break;
		
		case gestaltReactorCoreAddr:
			*response = (long) &the_reactor_core;
			break;
		
		case gestaltMachineIcon:
			*response = 42;
			break;
		
		case gestaltUserVisibleMachineName:
			*(unsigned char**) response = "\p" "Advanced Mac Substitute";
			break;
		
		default:
			return selector;
	}
	
	return 0;
}

asm
short Gestalt_patch( unsigned long selector : __D0 )
{
	MOVE.L   A1,-(SP)         // save A1, the 'response' address
	
	JSR      Gestalt_payload
	
	MOVEA.L  (SP)+,A1         // restore A1
	
	TST.L    D0
	BEQ.S    found
	
	// Not found -- try the native implementation.
	MOVEA.L  old_Gestalt,A0
	JMP      (A0)
	
found:
	// Found -- move the result to A0, where the glue code expects to find it.
	MOVEA.L  (A1),A0
	
	RTS
}
