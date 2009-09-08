// EPPC.cp

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#include "ClassicToolbox/EPPC.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( HighLevelEventManager )
	
	
	static void RegisterHighLevelEventManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class HighLevelEventManagerErrorsRegistration
	{
		public:
			HighLevelEventManagerErrorsRegistration()  { RegisterHighLevelEventManagerErrors(); }
	};
	
	static HighLevelEventManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	void RegisterHighLevelEventManagerErrors()
	{
		// FIXME
	}
	
}

