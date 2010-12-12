/*	===========
 *	OTSocket.hh
 *	===========
 */

#ifndef GENIE_IO_OTSOCKET_HH
#define GENIE_IO_OTSOCKET_HH

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	boost::shared_ptr< IOHandle > New_OT_Socket( bool nonblocking );
	
}

#endif

