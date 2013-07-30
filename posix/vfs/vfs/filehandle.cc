/*
	filehandle.cc
	-------------
*/

#include "vfs/filehandle.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	filehandle::filehandle( int                           flags,
	                        const filehandle_method_set*  methods )
	:
		its_flags  ( flags   ),
		its_methods( methods )
	{
	}
	
	filehandle::filehandle( const node*                   file,
	                        int                           flags,
	                        const filehandle_method_set*  methods )
	:
		its_file   ( file    ),
		its_flags  ( flags   ),
		its_methods( methods )
	{
	}
	
	filehandle::~filehandle()
	{
	}
	
	const socket_method_set& filehandle::socket_methods() const
	{
		if ( !its_methods  ||  !its_methods->socket_methods )
		{
			p7::throw_errno( ENOTSOCK );
		}
		
		return *its_methods->socket_methods;
	}
	
}

