// Components.cp

#ifndef NITROGEN_COMPONENTS_H
#include "Nitrogen/Components.h"
#endif

namespace Nitrogen {
	
	Owned< ComponentInstance >
	OpenDefaultComponent( ComponentType componentType, ComponentSubType componentSubType )
	{
		ComponentInstance component = ::OpenDefaultComponent( componentType, componentSubType );
		if ( component == NULL )
		{
			throw ErrorCode< OSStatus, ::cantFindHandler >();
		}
		return Owned< ComponentInstance >::Seize( component );
	}
	
}

