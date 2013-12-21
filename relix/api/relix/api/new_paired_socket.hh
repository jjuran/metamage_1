/*
	new_paired_socket.hh
	--------------------
*/

#ifndef RELIX_API_NEWPAIREDSOCKET_HH
#define RELIX_API_NEWPAIREDSOCKET_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// vfs
#include "vfs/stream.hh"


namespace relix
{
	
	vfs::filehandle_ptr new_paired_socket( const boost::intrusive_ptr< vfs::stream >&  input,
	                                       const boost::intrusive_ptr< vfs::stream >&  output,
	                                       bool                                        nonblocking );
	
}

#endif

