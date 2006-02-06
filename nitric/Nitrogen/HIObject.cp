// HIObject.cp

#ifndef NITROGEN_HIOBJECT_H
#include "Nitrogen/HIObject.h"
#endif

#include "Nitrogen/MacErrors.h"

namespace Nitrogen {

  	void RegisterHIObjectErrors () {
		RegisterOSStatus< hiObjectClassExistsErr        >();
		RegisterOSStatus< hiObjectClassHasInstancesErr  >();
		RegisterOSStatus< hiObjectClassHasSubclassesErr >();
		RegisterOSStatus< hiObjectClassIsAbstractErr    >();
		}

	}
