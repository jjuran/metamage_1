/*
	filehandle.cc
	-------------
*/

#include "vfs/filehandle.hh"


namespace vfs
{
	
	filehandle::filehandle( int flags ) : its_flags( flags )
	{
	}
	
	filehandle::~filehandle()
	{
	}
	
}

