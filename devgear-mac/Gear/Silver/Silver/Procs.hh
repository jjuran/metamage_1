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
	#define ProcPtrName(   name )  name ## ProcPtr
	#define DefineProcPtr( n    )  typedef ProcName( n )::ProcPtr ProcPtrName( n )
	
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
	typedef ProcType1< void, long    > ProcName( SystemMenu   );
	
	DefineProcPtr( DrawText      );
	DefineProcPtr( GetNextEvent  );
	DefineProcPtr( InsertMenu    );
	DefineProcPtr( MenuKey       );
	DefineProcPtr( MenuSelect    );
	DefineProcPtr( StillDown     );
	DefineProcPtr( TEActivate    );
	DefineProcPtr( TEClick       );
	DefineProcPtr( TEKey         );
	DefineProcPtr( TEPaste       );
	DefineProcPtr( AppendResMenu );
	DefineProcPtr( TESetSelect   );
	DefineProcPtr( SystemMenu   );
	
	// Other
	
	typedef ProcType5< void, short, MenuHandle, Rect*, Point, short* >  ProcName( MDEF );
	
	DefineProcPtr( MDEF );
	
	#undef ProcName
	#undef ProcPtrName
	#undef DefineProcPtr
	
}

#endif

