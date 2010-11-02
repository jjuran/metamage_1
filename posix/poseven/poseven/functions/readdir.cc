/*
	readdir.cc
	----------
*/

#include "poseven/functions/readdir.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	const dirent& readdir( dir_t dir )
	{
		const dirent* entry = ::readdir( dir );
		
		if ( entry == 0 )  // NULL
		{
			throw_errno( errno );
		}
		
		return *entry;
	}
	
}

