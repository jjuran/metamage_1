/*
	Mac/Lists/Types/ListHandle.hh
	-----------------------------
*/

#ifndef MAC_LISTS_TYPES_LISTHANDLE_HH
#define MAC_LISTS_TYPES_LISTHANDLE_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __LISTS__
#include <Lists.h>
#endif

// nucleus
#ifndef NUCLEUS_DISPOSER_HH
#include "nucleus/disposer.hh"
#endif


namespace nucleus
{
	
#if ! __LP64__
	
	template <> struct disposer< ListHandle >
	{
		typedef ListHandle  argument_type;
		typedef void        result_type;
		
		void operator()( ListHandle list ) const
		{
			::LDispose( list );
		}
	};
	
#endif  // #if ! __LP64__
	
}

#endif
