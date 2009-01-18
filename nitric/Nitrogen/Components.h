// Nitrogen/Components.h
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_COMPONENTS_H
#define NITROGEN_COMPONENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __COMPONENTS__
#include FRAMEWORK_HEADER(CarbonCore,Components.h)
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#include "Nucleus/Enumeration.h"
#include "Nucleus/ErrorsRegistered.h"


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ComponentManager );
	
	enum ComponentType
	{
		kAnyComponentType = ::kAnyComponentType,
		
		kComponentType_Max = Nucleus::Enumeration_Traits< ::OSType >::max
	};
	
	enum ComponentSubType
	{
		kAnyComponentSubType = ::kAnyComponentSubType,
		
		kComponentSubType_Max = Nucleus::Enumeration_Traits< ::OSType >::max
	};
	
	enum ComponentManufacturer
	{
		kAppleManufacturer        = ::kAppleManufacturer,
		kAnyComponentManufacturer = ::kAnyComponentManufacturer,
		
		kComponentManufacturer_Max = Nucleus::Enumeration_Traits< ::OSType >::max
	};
	
	static const ResType kComponentResourceType      = ResType( ::kComponentResourceType      );
	static const ResType kComponentAliasResourceType = ResType( ::kComponentAliasResourceType );
	
	using ::ComponentInstance;
	
}

namespace Nucleus
{
	
	template <>
	struct Disposer< Nitrogen::ComponentInstance > : public std::unary_function< Nitrogen::ComponentInstance, void >,
	                                                 private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::ComponentInstance component ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::ComponentManager );
			
			HandleDestructionOSStatus( ::CloseComponent( component ) );
		}
	};
	
}

namespace Nitrogen
{
	
	struct OpenDefaultComponent_Failed  {};
	
	Nucleus::Owned< ComponentInstance > OpenDefaultComponent( ComponentType     componentType,
	                                                          ComponentSubType  componentSubType );
	
	inline void CloseComponent( Nucleus::Owned< ComponentInstance > )  {}
	
}

#endif

