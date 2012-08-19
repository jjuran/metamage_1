/*
	filehandle.cc
	-------------
*/

#include "vfs/filehandle.hh"


namespace vfs
{
	
	filehandle::filehandle( int                           flags,
	                        const filehandle_method_set*  methods )
	:
		its_flags  ( flags   ),
		its_methods( methods )
	{
	}
	
	filehandle::~filehandle()
	{
	}
	
}

