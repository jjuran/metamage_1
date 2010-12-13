/*
	Mac/OpenTransport/Utilities/OTNotifier_Entrance.hh
	--------------------------------------------------
*/

#ifndef MAC_OPENTRANSPORT_UTILITIES_OTNOTIFIERENTRANCE_HH
#define MAC_OPENTRANSPORT_UTILITIES_OTNOTIFIERENTRANCE_HH

// Mac OS
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif


namespace Mac
{
	
	class OTNotifier_Entrance
	{
		private:
			EndpointRef its_endpoint;
		
		private:
			// non-copyable
			OTNotifier_Entrance           ( const OTNotifier_Entrance& );
			OTNotifier_Entrance& operator=( const OTNotifier_Entrance& );
		
		public:
			OTNotifier_Entrance( EndpointRef endpoint );
			
			~OTNotifier_Entrance();
	};
	
	inline OTNotifier_Entrance::OTNotifier_Entrance( EndpointRef endpoint )
	:
		its_endpoint( ::OTEnterNotifier( endpoint ) ? endpoint
		                                            : NULL )
	{
	}
	
	inline OTNotifier_Entrance::~OTNotifier_Entrance()
	{
		if ( its_endpoint )
		{
			::OTLeaveNotifier( its_endpoint );
		}
	}
	
}

#endif

