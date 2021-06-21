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

// more-libc
#include "more/string.h"

// mac-sys-utils
#ifdef __MACOS__
#include "mac_sys/has/RealTempMemory.hh"
#endif


#pragma exceptions off


namespace relix
{
	
#ifdef __MACOS__
	
	using mac::sys::has_RealTempMemory;
	
#else
	
	static inline
	char** TempNewHandle( unsigned long, short* )
	{
		return NULL;
	}
	
	static inline
	char** NewHandle( unsigned long )
	{
		return NULL;
	}
	
	static inline
	void DisposeHandle( char** )
	{
	}
	
	static inline
	bool has_RealTempMemory()
	{
		return false;
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
		
		if ( has_RealTempMemory() )
		{
			typedef short OSErr;
			
			OSErr err;
			
			h = TempNewHandle( size, &err );
			
			if ( h != NULL )
			{
				return Allocation_successful;
			}
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
			mempcpy( *h, canon, size );
		}
	}
	
	void private_mmap_swap::restore( void* canon, unsigned long size ) const
	{
		if ( h != NULL )
		{
			mempcpy( canon, *h, size );
		}
	}
	
}
