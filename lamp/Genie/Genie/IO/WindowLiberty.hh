/*	================
 *	WindowLiberty.hh
 *	================
 */

#ifndef GENIE_IO_WINDOWLIBERTY_HH
#define GENIE_IO_WINDOWLIBERTY_HH

// Boost
#include <boost/shared_ptr.hpp>

// Pedestal
#include "Pedestal/Window.hh"


namespace Genie
{
	
	class IOHandle;
	
	void LiberateWindow( Pedestal::ClosableWindow&             closable,
	                     ::WindowRef                           windowRef,
	                     const boost::shared_ptr< IOHandle >&  window );
	
}

#endif

