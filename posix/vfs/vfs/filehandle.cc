/*
	filehandle.cc
	-------------
*/

#include "vfs/filehandle.hh"


namespace vfs
{
	
	filehandle::filehandle( int flags ) : itsOpenFlags( flags )
	{
	}
	
	filehandle::~filehandle()
	{
	}
	
}

