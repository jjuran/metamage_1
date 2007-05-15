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
	
	#define ProcName(      name )  name ## Proc
	
	#define DEFINE_PROCPTR( name )  typedef name ## Proc::ProcPtr name ## ProcPtr
	
	// Traps
	
	typedef ProcType3< void, const void*, short, short > ProcName( DrawText      );
	typedef ProcType2< short, EventMask, EventRecord*  > ProcName( GetNextEvent  );
	typedef ProcType2< void, MenuHandle, short         > ProcName( InsertMenu    );
	typedef ProcType1< long, short                     > ProcName( MenuKey       );
	typedef ProcType1< long, Point                     > ProcName( MenuSelect    );
	typedef ProcType0< Boolean                         > ProcName( StillDown     );
	typedef ProcType1< void, TEHandle                  > ProcName( TEActivate    );
	typedef ProcType3< void, Point, short, TEHandle    > ProcName( TEClick       );
	typedef ProcType2< void, short, TEHandle           > ProcName( TEKey         );
	typedef ProcType1< void, TEHandle                  > ProcName( TEPaste       );
	typedef ProcType2< void, MenuRef, ResType          > ProcName( AppendResMenu );
	typedef ProcType3< void, short, short, TEHandle    > ProcName( TESetSelect   );
	typedef ProcType1< void, long                      > ProcName( SystemMenu    );
	typedef ProcType0< void                            > ProcName( ExitToShell   );
	
	DEFINE_PROCPTR( DrawText      );
	DEFINE_PROCPTR( GetNextEvent  );
	DEFINE_PROCPTR( InsertMenu    );
	DEFINE_PROCPTR( MenuKey       );
	DEFINE_PROCPTR( MenuSelect    );
	DEFINE_PROCPTR( StillDown     );
	DEFINE_PROCPTR( TEActivate    );
	DEFINE_PROCPTR( TEClick       );
	DEFINE_PROCPTR( TEKey         );
	DEFINE_PROCPTR( TEPaste       );
	DEFINE_PROCPTR( AppendResMenu );
	DEFINE_PROCPTR( TESetSelect   );
	DEFINE_PROCPTR( SystemMenu    );
	DEFINE_PROCPTR( ExitToShell   );
	
	// Other
	
	typedef ProcType5< void, short, MenuHandle, Rect*, Point, short* >  ProcName( MDEF );
	
	DEFINE_PROCPTR( MDEF );
	
	#undef DEFINE_PROCPTR
	
	#undef ProcName
	
}

#endif

