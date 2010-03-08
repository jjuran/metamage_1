// OSErrno.hh

#ifndef OSERRNO_HH
#define OSERRNO_HH

// Nitrogen
#include "Nitrogen/OSStatus.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace OSErrno
{
	
	poseven::errno_t ErrnoFromOSStatus( const Nitrogen::OSStatus& error );
	
}

#endif

