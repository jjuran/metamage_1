/*
	
	Procs.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PROCS_HH
#define SILVER_PROCS_HH

#include <Events.h>
#include <Menus.h>
#include <Resources.h>
#include <TextEdit.h>


namespace Silver
{
	
	#define DEFINE_PROCPTR( name, result, args )  typedef pascal result (*name ## ProcPtr) args
	
	namespace Trap_ProcPtrs
	{
		
		DEFINE_PROCPTR( DrawText,      void,    (const void*, short,       short   ) );
		DEFINE_PROCPTR( GetNextEvent,  short,   (EventMask,   EventRecord*         ) );
		DEFINE_PROCPTR( InsertMenu,    void,    (MenuHandle,  short                ) );
		DEFINE_PROCPTR( MenuKey,       long,    (short                             ) );
		DEFINE_PROCPTR( MenuSelect,    long,    (Point                             ) );
		DEFINE_PROCPTR( StillDown,     Boolean, (                                  ) );
		DEFINE_PROCPTR( TEActivate,    void,    (TEHandle                          ) );
		DEFINE_PROCPTR( TEClick,       void,    (Point,       short,       TEHandle) );
		DEFINE_PROCPTR( TEKey,         void,    (short,       TEHandle             ) );
		DEFINE_PROCPTR( TEPaste,       void,    (TEHandle                          ) );
		DEFINE_PROCPTR( AppendResMenu, void,    (MenuRef,     ResType              ) );
		DEFINE_PROCPTR( TESetSelect,   void,    (short,       short,       TEHandle) );
		DEFINE_PROCPTR( SystemMenu,    void,    (long                              ) );
		DEFINE_PROCPTR( ExitToShell,   void,    (                                  ) );
		
	}
	
	using namespace Trap_ProcPtrs;
	
	#undef DEFINE_PROCPTR
	
}

#endif

