/*	=====================
 *	ShareOpenTransport.cc
 *	=====================
 */

#include "Genie/Utilities/ShareOpenTransport.hh"

// Boost
#include <boost/weak_ptr.hpp>

// Debug
#include "debug/assert.hh"

// ClassicToolbox
#include "ClassicToolbox/OpenTransport.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	class OpenTransport
	{
		private:
			// Non-copyable
			OpenTransport           ( const OpenTransport& );
			OpenTransport& operator=( const OpenTransport& );
			
			// Enforce singularity
			static bool itIsInitialized;
		
		public:
			OpenTransport()
			{
				ASSERT( !itIsInitialized );
				
				N::InitOpenTransport();
				
				itIsInitialized = true;
			}
			
			~OpenTransport()
			{
				ASSERT( itIsInitialized );
				
				N::CloseOpenTransport();
				
				itIsInitialized = false;
			}
	};
	
	bool OpenTransport::itIsInitialized = false;
	
	
	static boost::shared_ptr< OpenTransport > GetSharedOpenTransport()
	{
		static boost::weak_ptr< OpenTransport > gOpenTransport;
		
		if ( gOpenTransport.expired() )
		{
			boost::shared_ptr< OpenTransport > sharedOpenTransport( new OpenTransport() );
			
			gOpenTransport = sharedOpenTransport;
			
			return sharedOpenTransport;
		}
		
		return boost::shared_ptr< OpenTransport >( gOpenTransport );
	}
	
	OpenTransportShare::OpenTransportShare() : itsShare( GetSharedOpenTransport() )
	{
	}
	
	OpenTransportShare::~OpenTransportShare()
	{
	}
	
}

