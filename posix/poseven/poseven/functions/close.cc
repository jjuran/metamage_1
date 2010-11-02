/*
	close.cc
	--------
*/


#include "poseven/functions/close.hh"


namespace poseven
{
	
	void close( nucleus::owned< fd_t > fd )
	{
		throw_posix_result( ::close( fd.release() ) );
	}
	
}

