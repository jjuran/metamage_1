/*
	Mac/AppleEvents/Utilities/NonNull_AEDescs_Are_Live.hh
	-----------------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_UTILITIES_NONNULLAEDESCSARELIVE_HH
#define MAC_APPLEEVENTS_UTILITIES_NONNULLAEDESCSARELIVE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif


namespace Mac
{
	
	class NonNull_AEDescs_Are_Live
	{
		public:
			static bool is_live( const AEDesc& desc )
			{
				return desc.dataHandle != NULL;
			}
	};
	
}

#endif

