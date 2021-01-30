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


#pragma exceptions off


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
	
	allocation_result private_mmap_swap::allocate( unsigned long size )
	{
		if ( allocated() )
		{
			return Allocation_unnecessary;
		}
		
		typedef short OSErr;
		
		OSErr err;
		
		h = TempNewHandle( size, &err );
		
		if ( h != NULL )
		{
			return Allocation_successful;
		}
		
		h = NewHandle( size );
		
		return (allocation_result) allocated();
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
