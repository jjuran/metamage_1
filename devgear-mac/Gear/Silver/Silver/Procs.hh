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

// Silver
#include "Silver/ProcTypes.hh"


namespace Silver
{
	
	#define PROCPTR_NAME( name )  name ## ProcPtr
	
	// Traps
	
	typedef ProcType3< void, const void*, short, short >::ProcPtr PROCPTR_NAME( DrawText      );
	typedef ProcType2< short, EventMask, EventRecord*  >::ProcPtr PROCPTR_NAME( GetNextEvent  );
	typedef ProcType2< void, MenuHandle, short         >::ProcPtr PROCPTR_NAME( InsertMenu    );
	typedef ProcType1< long, short                     >::ProcPtr PROCPTR_NAME( MenuKey       );
	typedef ProcType1< long, Point                     >::ProcPtr PROCPTR_NAME( MenuSelect    );
	typedef ProcType0< Boolean                         >::ProcPtr PROCPTR_NAME( StillDown     );
	typedef ProcType1< void, TEHandle                  >::ProcPtr PROCPTR_NAME( TEActivate    );
	typedef ProcType3< void, Point, short, TEHandle    >::ProcPtr PROCPTR_NAME( TEClick       );
	typedef ProcType2< void, short, TEHandle           >::ProcPtr PROCPTR_NAME( TEKey         );
	typedef ProcType1< void, TEHandle                  >::ProcPtr PROCPTR_NAME( TEPaste       );
	typedef ProcType2< void, MenuRef, ResType          >::ProcPtr PROCPTR_NAME( AppendResMenu );
	typedef ProcType3< void, short, short, TEHandle    >::ProcPtr PROCPTR_NAME( TESetSelect   );
	typedef ProcType1< void, long                      >::ProcPtr PROCPTR_NAME( SystemMenu    );
	typedef ProcType0< void                            >::ProcPtr PROCPTR_NAME( ExitToShell   );
	
	// Other
	
	typedef ProcType5< void, short, MenuHandle, Rect*, Point, short* >::ProcPtr  PROCPTR_NAME( MDEF );
	
	#undef PROCPTR_NAME
	
}

#endif

