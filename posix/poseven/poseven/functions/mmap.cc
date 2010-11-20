/*
	mmap.cc
	-------
*/

#include "poseven/functions/mmap.hh"


namespace poseven
{
	
	nucleus::owned< mmap_t > mmap( size_t        len,
	                               prot_t        prot,
	                               mmap_flags_t  flags,
	                               fd_t          fd,
	                               off_t         offset )
	{
		void* addr = ::mmap( NULL, len, prot, flags, fd, offset );
		
		if ( addr == MAP_FAILED )
		{
			throw_errno( errno );
		}
		
		mmap_t m = { addr, len };
		
		return nucleus::owned< mmap_t >::seize( m );
	}
	
}

