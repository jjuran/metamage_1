/*
	Mac/Icons/Types/IconRef.hh
	--------------------------
*/

#ifndef MAC_ICONS_TYPES_ICONREF_HH
#define MAC_ICONS_TYPES_ICONREF_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif

// nucleus
#ifndef NUCLEUS_DISPOSER_HH
#include "nucleus/disposer.hh"
#endif


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

