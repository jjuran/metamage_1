/*
	Mac/Resources/Types/ResType.hh
	------------------------------
*/

#ifndef MAC_RESOURCES_TYPES_RESTYPE_HH
#define MAC_RESOURCES_TYPES_RESTYPE_HH

#ifndef __ALIASES__
#include <Aliases.h>
#endif
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif
#ifndef __COMPONENTS__
#include <Components.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif
#ifndef __OSA__
#include <OSA.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum ResType
	{
		kVersionResType = 'vers',
		
		// Aliases
		// -------
		
		rAliasType = ::rAliasType,
		
		// CodeFragments
		// -------------
		
		kCFragResourceType = ::kCFragResourceType,
		
		// Components
		// ----------
		
		kComponentResourceType      = ::kComponentResourceType,
		kComponentAliasResourceType = ::kComponentAliasResourceType,
		
		// OSA
		// ---
		
		kOSAScriptResourceType = ::kOSAScriptResourceType,
		
		kResType_Max = nucleus::enumeration_traits< ::ResType >::max
	};
	
}

#endif

