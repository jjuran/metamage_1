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
	gestaltAddressingModeAttr     = 'addr',
	gestaltAppleTalkVersion       = 'atlk',
	gestaltFPUType                = 'fpu ',
	gestaltMachineIcon            = 'micn',
	gestaltUserVisibleMachineName = 'mnam',
	gestaltMMUType                = 'mmu ',
	gestaltParityAttr             = 'prty',
	gestaltOSTable                = 'ostt',
	gestaltQuickdrawVersion       = 'qd  ',
	gestaltSerialAttr             = 'ser ',
	gestaltNuBusConnectors        = 'sltc',
	gestaltSystemVersion          = 'sysv',
	gestaltToolboxTable           = 'tbtt',
	gestaltVersion                = 'vers',
	gestaltVMAttr                 = 'vm  ',
};

short SysVersion : 0x015A;

Gestalt_ProcPtr old_Gestalt;

static
long Gestalt_payload( unsigned long  selector : __D0,
                      long*          response : __A1 )
{
	switch ( selector )
	{
		case gestaltAddressingModeAttr:
		case gestaltAppleTalkVersion:
		case gestaltFPUType:
		case gestaltMMUType:
		case gestaltParityAttr:
		case gestaltQuickdrawVersion:
		case gestaltSerialAttr:
		case gestaltNuBusConnectors:
		case gestaltVersion:
		case gestaltVMAttr:
			*response = 0;
			break;
		
		case gestaltReactorCoreAddr:
			*response = (long) &the_reactor_core;
			break;
		
		case gestaltOSTable:
			*response = 0x00000400;  // 1K
			break;
		
		case gestaltToolboxTable:
			*response = 0x00000c00;  // 3K
			break;
		
		case gestaltMachineIcon:
			*response = 42;
			break;
		
		case gestaltUserVisibleMachineName:
			*(unsigned char**) response = "\p" "Advanced Mac Substitute";
			break;
		
		case gestaltSystemVersion:
			*response = SysVersion;
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
