/*
	Mac/Lists/Types/ListHandle.hh
	-----------------------------
*/

#ifndef MAC_LISTS_TYPES_LISTHANDLE_HH
#define MAC_LISTS_TYPES_LISTHANDLE_HH

#ifndef __LISTS__
#include <Lists.h>
#endif

// nucleus
#include "nucleus/disposer.hh"


namespace nucleus
{
	
	template <> struct disposer< ListHandle >
	{
		typedef ListHandle  argument_type;
		typedef void        result_type;
		
		void operator()( ListHandle list ) const
		{
			::LDispose( list );
		}
	};
	
}

#endif

