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
#ifndef NUCLEUS_SELECTORTYPE_H
#include "Nucleus/SelectorType.h"
#endif
#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	void RegisterComponentManagerErrors();
	
	struct ComponentTypeTag  {};
	typedef Nucleus::SelectorType< ComponentTypeTag, ::OSType, kAnyComponentType > ComponentType;
	
	struct ComponentSubTypeTag  {};
	typedef Nucleus::SelectorType< ComponentSubTypeTag, ::OSType, kAnyComponentSubType > ComponentSubType;
	
	struct ComponentManufacturerTag  {};
	typedef Nucleus::SelectorType< ComponentManufacturerTag, ::OSType, kAnyComponentManufacturer > ComponentManufacturer;
	
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
			Nucleus::OnlyOnce< Nitrogen::RegisterComponentManagerErrors >();
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

