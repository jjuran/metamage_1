/*
	closedir.cc
	-----------
*/


#include "poseven/functions/closedir.hh"


namespace poseven
{
	
	void closedir( nucleus::owned< dir_t > dir )
	{
		throw_posix_result( ::closedir( dir.release() ) );
	}
	
}

