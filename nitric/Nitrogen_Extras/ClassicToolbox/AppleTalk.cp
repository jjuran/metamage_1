// AppleTalk.cp

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#ifndef CLASSICTOOLBOX_APPLETALK_H
#include "ClassicToolbox/AppleTalk.h"
#endif

// Nitrogen
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	void RegisterAppleTalkErrors()
	{
		RegisterOSStatus< paramErr  >();
		RegisterOSStatus< portInUse >();
		RegisterOSStatus< portNotCf >();
		RegisterOSStatus< noMPPErr  >();
	}
	
}

