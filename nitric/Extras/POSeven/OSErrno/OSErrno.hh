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

namespace Nucleus
{
	
	template <>
	struct Converter< poseven::errno_t, Nitrogen::OSStatus > : public std::unary_function< Nitrogen::OSStatus, poseven::errno_t >
	{
		poseven::errno_t operator()( Nitrogen::OSStatus error ) const
		{
			int result = OSErrno::ErrnoFromOSStatus( error );
			
			return poseven::errno_t( result );
		}
	};
	
}

#endif

