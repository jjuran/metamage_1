// AppleTalk.cp

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#include "ClassicToolbox/AppleTalk.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( AppleTalk )
	
	
	static void RegisterAppleTalkErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class AppleTalkErrorsRegistration
	{
		public:
			AppleTalkErrorsRegistration()  { RegisterAppleTalkErrors(); }
	};
	
	static AppleTalkErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	void RegisterAppleTalkErrors()
	{
		RegisterOSStatus< paramErr  >();
		RegisterOSStatus< portInUse >();
		RegisterOSStatus< portNotCf >();
		RegisterOSStatus< noMPPErr  >();
	}
	
}

