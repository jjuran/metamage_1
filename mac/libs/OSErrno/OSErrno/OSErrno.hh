// OSErrno.hh

#ifndef OSERRNO_HH
#define OSERRNO_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// poseven
#ifndef POSEVEN_TYPES_ERRNO_T_HH
#include "poseven/types/errno_t.hh"
#endif


namespace OSErrno
{
	
	poseven::errno_t ErrnoFromOSStatus( ::OSStatus error );
	
}

#endif

