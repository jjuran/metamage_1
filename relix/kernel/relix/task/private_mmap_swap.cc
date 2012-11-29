/*
	private_mmap_swap.cc
	--------------------
*/

#include "relix/task/private_mmap_swap.hh"

// Mac OS
#ifdef __MACOS__
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#endif

// Standard C
#include <string.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace relix
{
	
#ifndef __MACOS__
	
	static inline char** TempNewHandle( unsigned long, short* )
	{
		return NULL;
	}
	
	static inline void DisposeHandle( char** )
	{
	}
	
#endif
	
	private_mmap_swap::~private_mmap_swap()
	{
		DisposeHandle( h );
	}
	
	void private_mmap_swap::allocate( unsigned long size )
	{
		typedef short OSErr;
		
		OSErr err;
		
		h = TempNewHandle( size, &err );
		
		if ( h == NULL )
		{
			poseven::throw_errno( ENOMEM );
		}
	}
	
	void private_mmap_swap::reset()
	{
		DisposeHandle( h );
		
		h = NULL;
	}
	
	void private_mmap_swap::back_up( const void* canon, unsigned long size ) const
	{
		if ( h != NULL )
		{
			memcpy( *h, canon, size );
		}
	}
	
	void private_mmap_swap::restore( void* canon, unsigned long size ) const
	{
		if ( h != NULL )
		{
			memcpy( canon, *h, size );
		}
	}
	
}
