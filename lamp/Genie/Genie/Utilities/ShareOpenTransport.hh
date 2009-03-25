/*	=====================
 *	ShareOpenTransport.hh
 *	=====================
 */

#ifndef SHAREOPENTRANSPORT_HH
#define SHAREOPENTRANSPORT_HH

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class OpenTransport;
	
	class OpenTransportShare
	{
		private:
			boost::shared_ptr< OpenTransport > itsShare;
		
		public:
			OpenTransportShare();
			~OpenTransportShare();
	};
	
	
	inline OpenTransportShare ShareOpenTransport()  { return OpenTransportShare(); }
	
}

#endif

