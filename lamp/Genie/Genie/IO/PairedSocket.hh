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

// vfs
#include "vfs/stream.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	IOPtr
	//
	NewPairedSocket( const boost::intrusive_ptr< vfs::stream >&  input,
	                 const boost::intrusive_ptr< vfs::stream >&  output,
	                 bool                                        nonblocking );
	
}

#endif

