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

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

// GetPathname
#include "GetMacPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

#endif


namespace p7 = poseven;


plus::string mac_pathname_from_path( const char* path, bool utf8 )
{
#ifdef __APPLE__
	
	namespace Div = Divergence;
	
	try
	{
		return GetMacPathname( Div::ResolvePathToFSSpec( path ), utf8 );
	}
	catch ( const Mac::OSStatus& err )
	{
		// * Div::ResolvePathToFSSpec() only throws OSStatus on Mach
		// * GetMacPathname() should only ever throw fnfErr
		
		// Don't bother reporting the OSStatus, just convert and rethrow
		
		throw p7::errno_t( OSErrno::ErrnoFromOSStatus( err ) );
	}
	
#endif
	
#ifdef __RELIX__
	
	char buffer[ 4096 ];
	
	const int flags = REALPATH_OUTPUT_HFS | utf8 * REALPATH_OUTPUT_UTF8;
	
	ssize_t size = _realpathat( AT_FDCWD, path, buffer, sizeof buffer, flags );
	
	if ( size < 0 )
	{
		p7::throw_errno( errno );
	}
	
	return plus::string( buffer, size );
	
#endif
}
