/*	================
 *	WindowLiberty.hh
 *	================
 */

#ifndef GENIE_IO_WINDOWLIBERTY_HH
#define GENIE_IO_WINDOWLIBERTY_HH

// Boost
#include <boost/shared_ptr.hpp>


namespace Pedestal
{
	
	class WindowCore;
	
}

namespace Genie
{
	
	class IOHandle;
	
	void LiberateWindow( Pedestal::WindowCore&                 window,
	                     const boost::shared_ptr< IOHandle >&  handle );
	
}

#endif

