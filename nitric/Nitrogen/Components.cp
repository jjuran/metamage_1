// Components.cp

#ifndef NITROGEN_COMPONENTS_H
#include "Nitrogen/Components.h"
#endif

namespace Nitrogen
{
	
	void RegisterComponentManagerErrors()
	{
		RegisterOSStatus< resFNotFound         >();
		RegisterOSStatus< invalidComponentID   >();
		RegisterOSStatus< validInstancesExist  >();
		RegisterOSStatus< componentNotCaptured >();
		RegisterOSStatus< badComponentInstance >();
		RegisterOSStatus< badComponentSelector >();
	}
	
	Nucleus::Owned< ComponentInstance >
	OpenDefaultComponent( ComponentType     componentType,
	                      ComponentSubType  componentSubType )
	{
		Nucleus::OnlyOnce< RegisterComponentManagerErrors >();
		
		ComponentInstance component = ::OpenDefaultComponent( componentType,
		                                                      componentSubType );
		if ( component == NULL )
		{
			throw OpenDefaultComponent_Failed();
		}
		
		return Nucleus::Owned< ComponentInstance >::Seize( component );
	}
	
}

