// Components.h

#ifndef NITROGEN_COMPONENTS_H
#define NITROGEN_COMPONENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __COMPONENTS__
#include FRAMEWORK_HEADER(CarbonCore,Components.h)
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen {
	
	void RegisterComponentManagerErrors();
	
	struct ComponentTypeTag  {};
	typedef SelectorType< ComponentTypeTag, ::OSType, kAnyComponentType > ComponentType;
	
	struct ComponentSubTypeTag  {};
	typedef SelectorType< ComponentSubTypeTag, ::OSType, kAnyComponentSubType > ComponentSubType;
	
	struct ComponentManufacturerTag  {};
	typedef SelectorType< ComponentManufacturerTag, ::OSType, kAnyComponentManufacturer > ComponentManufacturer;
	
	using ::ComponentInstance;
	
	template <>
	struct Disposer< ComponentInstance > : public std::unary_function< ComponentInstance, void >,
	                                       private DefaultDestructionOSStatusPolicy
	{
		void operator()( ComponentInstance component ) const
		{
			OnlyOnce< RegisterComponentManagerErrors >();
			DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::CloseComponent( component ) );
		}
	};
	
	Owned< ComponentInstance >
	OpenDefaultComponent( ComponentType componentType, ComponentSubType componentSubType );
	
	inline void CloseComponent( Owned< ComponentInstance > )  {}
	
}

#endif

