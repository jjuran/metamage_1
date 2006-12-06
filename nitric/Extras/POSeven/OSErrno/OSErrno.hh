// OSErrno.hh

#ifndef OSERRNO_HH
#define OSERRNO_HH

// Nitrogen
#include "Nitrogen/OSStatus.h"

// POSeven
#include "POSeven/Errno.hh"


namespace OSErrno
{
	
	POSeven::Errno ErrnoFromOSStatus( const Nitrogen::OSStatus& error );
	
}

namespace Nucleus
{
	
	template <>
	struct Converter< POSeven::Errno, Nitrogen::OSStatus > : public std::unary_function< Nitrogen::OSStatus, POSeven::Errno >
	{
		POSeven::Errno operator()( Nitrogen::OSStatus error ) const
		{
			int result = OSErrno::ErrnoFromOSStatus( error );
			
			return POSeven::Errno( result );
		}
	};
	
}

#endif

