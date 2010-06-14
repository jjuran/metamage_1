// AEDataModel.hh

#ifndef CLASSICTOOLBOX_AEDATAMODEL_HH
#define CLASSICTOOLBOX_AEDATAMODEL_HH

#ifndef __EPPC__
#include <EPPC.h>
#endif

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


namespace Nitrogen
{
	
	// TargetID is defined for Carbon, but typeTargetID is not.
	
	template <> struct DescType_Traits< typeTargetID > : nucleus::POD_scribe< TargetID > {};
	
}

#endif

