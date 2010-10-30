// OSErrno.hh

#ifndef OSERRNO_HH
#define OSERRNO_HH

// Nitrogen
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

// poseven
#include "poseven/types/errno_t.hh"


namespace OSErrno
{
	
	poseven::errno_t ErrnoFromOSStatus( const Nitrogen::OSStatus& error );
	
}

#endif

