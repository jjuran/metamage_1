/*
	AuxDCE.hh
	---------
*/

#ifndef MACTYPES_AUXDCE_HH
#define MACTYPES_AUXDCE_HH

// Mac OS
//#include <Devices.h>
struct AuxDCE;


namespace mac   {
namespace types {
	
#pragma options align=mac68k
	
	struct AuxDCE
	{
		char*           dCtlDriver;    // Ptr
		volatile short  dCtlFlags;
		void*           dCtlQHdr;      // QHdr
		long            dCtlPosition;
		char**          dCtlStorage;   // Handle
		short           dCtlRefNum;
		long            dCtlCurTicks;
		void*           dCtlWindow;    // GrafPtr
		short           dCtlDelay;
		short           dCtlEMask;
		short           dCtlMenu;
		signed char     dCtlSlot;
		signed char     dCtlSlotId;
		long            dCtlDevBase;
		char*           dCtlOwner;     // Ptr
		signed char     dCtlExtDev;
		signed char     fillByte;
		unsigned long   dCtlNodeID;
		
		operator ::AuxDCE const&() const  { return *(::AuxDCE const*) this; }
		operator ::AuxDCE      &()        { return *(::AuxDCE      *) this; }
	};
	
	typedef AuxDCE*   AuxDCEPtr;
	typedef AuxDCE**  AuxDCEHandle;
	
#pragma options align=reset
	
}
}

#endif
