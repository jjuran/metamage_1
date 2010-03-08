/*
	mac_pathname_from_path.hh
	-------------------------
	
	Joshua Juran
*/

#include "mac_pathname_from_path.hh"

// poseven
#include "poseven/types/errno_t.hh"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"


namespace N = Nitrogen;
namespace p7 = poseven;
namespace Div = Divergence;


std::string mac_pathname_from_path( const char* path )
{
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
	
	// Not reached
	return std::string();
}
	
