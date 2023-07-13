/*
	Gestalt.hh
	----------
*/

#ifndef MACGLUE_GESTALT_HH
#define MACGLUE_GESTALT_HH

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#ifndef __MC68K__
#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#endif

namespace mac  {
namespace glue {
	
#ifdef __MC68K__
	
	typedef short OSErr;
	typedef long SInt32;
	
	typedef unsigned long OSType;
	
	enum
	{
		_Gestalt = 0xA1AD,
	};
	
	/*
		Like ::Gestalt(), mac::glue::gestalt_err() is glue
		for the _Gestalt trap that returns an OSErr in D0.
		Unlike it, it doesn't write a value into memory on 68K.
		
		gestalt_or() returns a value as its result, either from a
		successful Gestalt() call or from the alternative value
		passed as an argument.  Which case occurred is reflected
		in the CCR, and even the exact error code is preserved in
		the low word of A0 -- though a caller is unlikely to check.
	*/
	
	inline
	asm
	OSErr gestalt_err( OSType selector : __D0 )
	{
		_Gestalt
	}
	
	inline
	asm
	SInt32 gestalt_or( OSType selector : __D0, SInt32 alternative : __D1 )
	{
		_Gestalt
		
		// Move result to D0 while preserving CCR
		EXG      A0,D0
		
		BGE.S    non_negative
		
		// Gestalt failed; return alternative
		EXG      D1,D0
		
	non_negative:
	}
	
#else  // #ifdef __MC68K__
	
	inline
	OSErr gestalt_err( OSType selector )
	{
		SInt32 result;
		
		return Gestalt( selector, &result );
	}
	
	inline
	SInt32 gestalt_or( OSType selector, SInt32 alternative )
	{
		SInt32 result;
		
		OSErr err = Gestalt( selector, &result );
		
		if ( err )
		{
			result = alternative;
		}
		
		return result;
	}
	
#endif  // #else  // #ifdef __MC68K__
}
}

#endif
