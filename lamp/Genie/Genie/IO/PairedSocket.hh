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

// plus
#include "plus/conduit.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	IOPtr
	//
	NewPairedSocket( const boost::intrusive_ptr< plus::conduit >&  input,
	                 const boost::intrusive_ptr< plus::conduit >&  output,
	                 bool                                          nonblocking );
	
}

#endif

