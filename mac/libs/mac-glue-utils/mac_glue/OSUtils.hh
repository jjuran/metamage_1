/*
	OSUtils.hh
	----------
*/

#ifndef MACGLUE_OSUTILS_HH
#define MACGLUE_OSUTILS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MC68K__
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#endif


namespace mac  {
namespace glue {
	
#ifdef __MC68K__
	
	enum
	{
		_Delay = 0xA03B,
	};
	
	inline asm
	void delay( unsigned long deltaTicks : __A0 )
	{
		_Delay
	}
	
#else
	
	inline
	void delay( unsigned long deltaTicks )
	{
		unsigned long finalTicks;
		
		::Delay( deltaTicks, &finalTicks );
	}
	
#endif
	
}
}

#endif
