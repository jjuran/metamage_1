/*	====================
 *	CustomTEClickLoop.cc
 *	====================
 */

#include "Pedestal/CustomTEClickLoop.hh"

// Universal Interfaces
#include <TextEdit.h>

// Nucleus
#include "Nucleus/NAssert.h"

// Pedestal
//#include "Pedestal/Application.hh"
#include "Pedestal/Scroller.hh"


namespace Pedestal
{
	
	static void CustomClickLoop()
	{
		ClickableScroller::ClickLoop();
	}
	
	// This is set the first time we call SetCustomTEClickLoop().
	static ::TEClickLoopUPP gSystemClickLoop = NULL;
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	static pascal asm void MasterTEClickLoop()
	{
		MOVEA.L		gSystemClickLoop,A0	;  // load the default clickLoop
		
		JSR			(A0)				;  // invoke it
		
		MOVEM.L		D1-D2/A1,-(SP)		;  // save registers
		JSR			CustomClickLoop		;  // invoke our custom clickLoop
		MOVEM.L		(SP)+,D1-D2/A1		;  // restore registers
		
		MOVEQ		#1,D0				;  // return true by clearing the zero flag
		RTS
	}
	
#else
	
	static pascal Boolean MasterTEClickLoop( TEPtr pTE )
	{
		(void) ::InvokeTEClickLoopUPP( pTE, gSystemClickLoop );
		
		CustomClickLoop();
		
		return true;
	}
	
#endif
	
	static ::TEClickLoopUPP gMasterClickLoop = ::NewTEClickLoopUPP( MasterTEClickLoop );
	
	
	void InstallCustomTEClickLoop( TEHandle hTE )
	{
		static ::TEClickLoopUPP clickLoop = gSystemClickLoop = hTE[0]->clickLoop;
		
		hTE[0]->clickLoop = gMasterClickLoop;
	}
	
}

