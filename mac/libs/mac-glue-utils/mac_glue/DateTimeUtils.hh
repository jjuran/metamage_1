/*
	DateTimeUtils.hh
	----------------
*/

#ifndef MACGLUE_DATETIMEUTILS_HH
#define MACGLUE_DATETIMEUTILS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MC68K__
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif
#endif


namespace mac  {
namespace glue {

#ifndef __LP64__

inline
unsigned long get_Time()
{
#ifdef __MC68K__
	
	unsigned long& Time = *(unsigned long*) 0x020C;
	
#else
	
	unsigned long Time;
	
	GetDateTime( &Time );
	
#endif
	
	return Time;
}

#endif  // #ifndef __LP64__

}
}

#endif
