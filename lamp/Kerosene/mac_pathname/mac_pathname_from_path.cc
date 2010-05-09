/*
	mac_pathname_from_path.cc
	-------------------------
	
	Joshua Juran
*/

#include "mac_pathname_from_path.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"

#ifdef __APPLE__

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

#endif


namespace p7 = poseven;


plus::string mac_pathname_from_path( const char* path )
{
#ifdef __APPLE__
	
	namespace N = Nitrogen;
	namespace Div = Divergence;
	
	try
	{
		return GetMacPathname( Div::ResolvePathToFSSpec( path ) );
	}
	catch ( const N::OSStatus& err )
	{
		// * Div::ResolvePathToFSSpec() only throws OSStatus on Mach
		// * GetMacPathname() should only ever throw fnfErr
		
		// Don't bother reporting the OSStatus, just convert and rethrow
		
		p7::throw_errno( OSErrno::ErrnoFromOSStatus( err ) );
	}
	
#endif
	
#ifdef __LAMP__
	
	char buffer[ 4096 ];
	
	ssize_t size = _realpathat( AT_FDCWD, path, buffer, sizeof buffer, REALPATH_MAC );
	
	if ( size < 0 )
	{
		p7::throw_errno( errno );
	}
	
	return plus::string( buffer, size );
	
#endif
}
	
