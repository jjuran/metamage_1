/*	====================
 *	CustomTEClickLoop.cc
 *	====================
 */

#include "Pedestal/CustomTEClickLoop.hh"

// Universal Interfaces
#include <TextEdit.h>

// Debug
#include "debug/assert.hh"


namespace Pedestal
{
	
	static TEClickLoop_User   *  gCurrentUser    = NULL;
	static TEClickLoop_Subject*  gCurrentSubject = NULL;
	
	
	TEClickLoop_User::TEClickLoop_Scope::TEClickLoop_Scope( TEClickLoop_User* user )
	{
		ASSERT( gCurrentUser == NULL );
		
		gCurrentUser = user;
	}
	
	TEClickLoop_User::TEClickLoop_Scope::~TEClickLoop_Scope()
	{
		gCurrentUser = NULL;
	}
	
	TEClickLoop_Subject::TEClickLoop_Scope::TEClickLoop_Scope( TEClickLoop_Subject* subject )
	{
		ASSERT( gCurrentSubject == NULL );
		
		gCurrentSubject = subject;
	}
	
	TEClickLoop_Subject::TEClickLoop_Scope::~TEClickLoop_Scope()
	{
		gCurrentSubject = NULL;
	}
	
	static void CustomClickLoop()
	{
		if ( gCurrentSubject )
		{
			gCurrentSubject->ClickInLoop();
		}
		
		if ( gCurrentUser )
		{
			gCurrentUser->ClickInLoop();
		}
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
	
	
	void TEClickLoop_Subject::InstallCustomTEClickLoop( TEHandle hTE )
	{
		static ::TEClickLoopUPP clickLoop = gSystemClickLoop = hTE[0]->clickLoop;
		
		hTE[0]->clickLoop = gMasterClickLoop;
	}
	
}

