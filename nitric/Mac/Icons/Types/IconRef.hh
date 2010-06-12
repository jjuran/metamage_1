/*
	Mac/Icons/Types/IconRef.hh
	--------------------------
*/

#ifndef MAC_ICONS_TYPES_ICONREF_HH
#define MAC_ICONS_TYPES_ICONREF_HH

#ifndef __ICONS__
#include <Icons.h>
#endif

// nucleus
#include "nucleus/disposer.hh"


namespace nucleus
{
	
	template <> struct disposer< IconRef >
	{
		typedef IconRef  argument_type;
		typedef void     result_type;
		
		void operator()( IconRef icon ) const
		{
			::ReleaseIconRef( icon );
		}
	};
	
}

#endif

