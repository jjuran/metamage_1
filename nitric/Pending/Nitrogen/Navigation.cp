//	Navigation.h
#ifndef NITROGEN_NAVIGATION_H
#include "Nitrogen/Navigation.h"
#endif

namespace Nitrogen {

	void RegisterNavServicesErrors () {
		RegisterOSStatus< kNavWrongDialogStateErr            >();
		RegisterOSStatus< kNavWrongDialogClassErr            >();
		RegisterOSStatus< kNavInvalidSystemConfigErr         >();
		RegisterOSStatus< kNavCustomControlMessageFailedErr  >();
		RegisterOSStatus< kNavInvalidCustomControlMessageErr >();
		RegisterOSStatus< kNavMissingKindStringErr           >();
		}

	}
	
