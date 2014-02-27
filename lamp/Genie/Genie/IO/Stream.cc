/*	=========
 *	Stream.cc
 *	=========
 */

#include "Genie/IO/Stream.hh"

// vfs
#include "vfs/filehandle/functions/nonblocking.hh"

// Genie
#include "Genie/api/yield.hh"


namespace Genie
{
	
	StreamHandle::StreamHandle( int                                flags,
	                            const vfs::filehandle_method_set*  methods )
	:
		IOHandle( flags, methods )
	{
	}
	
	StreamHandle::StreamHandle( const vfs::node*                   file,
	                            int                                flags,
	                            const vfs::filehandle_method_set*  methods )
	:
		vfs::filehandle( file, flags, methods )
	{
	}
	
	StreamHandle::~StreamHandle()
	{
	}
	
	void StreamHandle::TryAgainLater() const
	{
		try_again( is_nonblocking( *this ) );
	}
	
}

