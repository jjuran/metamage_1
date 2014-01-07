/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	RegularFileHandle::RegularFileHandle( int                                flags,
	                                      const vfs::filehandle_method_set*  methods )
	:
		vfs::filehandle( flags, methods )
	{
	}
	
	RegularFileHandle::RegularFileHandle( const vfs::node_ptr&               file,
	                                      int                                flags,
	                                      const vfs::filehandle_method_set*  methods )
	:
		vfs::filehandle( file.get(), flags, methods )
	{
	}
	
	RegularFileHandle::~RegularFileHandle()
	{
	}
	
}

