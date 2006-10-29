// AEDataModel.h

#ifndef CLASSICTOOLBOX_AEDATAMODEL_H
#define CLASSICTOOLBOX_AEDATAMODEL_H

#ifndef __EPPC__
#include <EPPC.h>
#endif

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	// Deprecated addressing modes under Carbon
	static const DescType typeSessionID    = DescType( typeSessionID    );
	static const DescType typeTargetID     = DescType( typeTargetID     );
	static const DescType typeDispatcherID = DescType( typeDispatcherID );
	
#endif
	
   // TargetID is defined for Carbon, but typeTargetID is not.
#if CALL_NOT_IN_CARBON
	
	template <> struct DescType_Traits< ::typeTargetID > : POD_DescType_Traits< TargetID > {};
	
#endif
	
}

#endif

