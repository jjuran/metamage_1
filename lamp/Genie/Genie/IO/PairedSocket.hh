/*	===============
 *	PairedSocket.hh
 *	===============
 */

#ifndef GENIE_IO_PAIREDSOCKET_HH
#define GENIE_IO_PAIREDSOCKET_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Genie
#include "Genie/IO/Base.hh"
#include "Genie/IO/Conduit.hh"


namespace Genie
{
	
	IOPtr
	//
	NewPairedSocket( const boost::intrusive_ptr< conduit >&  input,
	                 const boost::intrusive_ptr< conduit >&  output,
	                 bool                                    nonblocking );
	
}

#endif

