/*
	Memory.hh
	---------
*/

#ifndef MACGLUE_MEMORY_HH
#define MACGLUE_MEMORY_HH

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#ifndef __MC68K__
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#endif

namespace mac  {
namespace glue {
	
	typedef char** Handle;
	
#ifdef __MC68K__
	
	enum
	{
		_GetHandleSize = 0xA025,
	};
	
	/*
		Like ::GetHandleSize(), mac::glue::GetHandleSize() is glue for the
		_GetHandleSize trap that replaces errors with zero.  Unlike it,
		it's register- rather than stack-based.
		
		GetHandleSize_raw() is similar, but doesn't /necessarily/ replace
		error values.  (It doesn't on 68K and does everywhere else.)
		It's useful (in that it's less code on 68K) when the calling logic
		handles either a size of zero or an error via the same code path.
	*/
	
	inline
	asm
	long GetHandleSize( Handle h : __A0 )
	{
		_GetHandleSize
		
		TST.L    D0     // OS traps only test W; we need L
		BGE.S    non_negative
		
		// D0 is negative; clear to zero
		MOVEQ    #0,D0
		
	non_negative:
	}
	
	inline
	asm
	long GetHandleSize_raw( Handle h : __A0 )
	{
		_GetHandleSize
	}
	
#else
	
	using ::GetHandleSize;
	
	inline
	long GetHandleSize_raw( Handle h )
	{
		return GetHandleSize( h );
	}
	
#endif
}
}

#endif
