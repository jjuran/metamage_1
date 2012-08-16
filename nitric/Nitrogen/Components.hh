// Nitrogen/Components.hh
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_COMPONENTS_HH
#define NITROGEN_COMPONENTS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __COMPONENTS__
#include <Components.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ComponentManager );
	
	enum ComponentType
	{
		kAnyComponentType = ::kAnyComponentType,
		
		kComponentType_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
	enum ComponentSubType
	{
		kAnyComponentSubType = ::kAnyComponentSubType,
		
		kComponentSubType_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
	enum ComponentManufacturer
	{
		kAppleManufacturer        = ::kAppleManufacturer,
		kAnyComponentManufacturer = ::kAnyComponentManufacturer,
		
		kComponentManufacturer_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
}

namespace nucleus
{
	
	template <>
	struct disposer< ComponentInstance >
	{
		typedef ComponentInstance  argument_type;
		typedef void               result_type;
		
		void operator()( ComponentInstance component ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::ComponentManager );
			
			(void) ::CloseComponent( component );
		}
	};
	
}

namespace Nitrogen
{
	
	struct OpenDefaultComponent_Failed  {};
	
	nucleus::owned< ComponentInstance > OpenDefaultComponent( ComponentType     componentType,
	                                                          ComponentSubType  componentSubType );
	
	inline void CloseComponent( nucleus::owned< ComponentInstance > )
	{
	}
	
}

#endif

