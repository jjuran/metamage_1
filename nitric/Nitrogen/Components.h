// Components.h

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
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	class ComponentManagerErrorsRegistrationDependency
	{
		public:
			ComponentManagerErrorsRegistrationDependency();
	};
	
	typedef Nucleus::Selector< class ComponentType_Tag,         ::OSType >::Type ComponentType;
	typedef Nucleus::Selector< class ComponentSubType_Tag,      ::OSType >::Type ComponentSubType;
	typedef Nucleus::Selector< class ComponentManufacturer_Tag, ::OSType >::Type ComponentManufacturer;
	
	static const ComponentManufacturer kAppleManufacturer = ComponentManufacturer( ::kAppleManufacturer );
	
	static const ResType kComponentResourceType      = ResType( ::kComponentResourceType      );
	static const ResType kComponentAliasResourceType = ResType( ::kComponentAliasResourceType );
	
	static const ComponentType         kAnyComponentType         = ComponentType        ( ::kAnyComponentType         );
	static const ComponentSubType      kAnyComponentSubType      = ComponentSubType     ( ::kAnyComponentSubType      );
	static const ComponentManufacturer kAnyComponentManufacturer = ComponentManufacturer( ::kAnyComponentManufacturer );
	
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
			(void) Nitrogen::ComponentManagerErrorsRegistrationDependency();
			
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

