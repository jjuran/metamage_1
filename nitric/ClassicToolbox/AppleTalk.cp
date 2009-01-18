// AppleTalk.cp

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#include "ClassicToolbox/AppleTalk.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


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

