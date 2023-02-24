/*
	mac_pathname_from_path.cc
	-------------------------
	
	Joshua Juran
*/

#include "mac_pathname_from_path.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

#ifdef __RELIX__

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// mac-file-utils
#include "mac_file/HFS_pathname.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

#endif

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


plus::string mac_pathname_from_path( const char* path )
{
#ifdef __APPLE__
	
	namespace Div = Divergence;
	
	try
	{
		return GetMacPathname( Div::ResolvePathToFSSpec( path ) );
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
	
	using mac::file::HFS_pathname;
	
	using mac::relix::FSSpec_from_optional_path;
	
	using mac::sys::Error;
	using mac::sys::errno_from_mac_error;
	
	plus::string result;
	
	FSSpec file;
	
	Error err = FSSpec_from_optional_path( path, file );
	
	if ( ! err )
	{
		long got = HFS_pathname( NULL, 0, file );
		
		if ( (SInt32) got <= (SInt32) 0x80008000 )
		{
			UInt32 length = (UInt16) got;
			
			char* p = result.reset( length );
			
			got = HFS_pathname( p, length, file );
		}
		
		if ( got < 0 )
		{
			err = Error( got );
		}
	}
	
	if ( err )
	{
		int errnum = is_errno( err ) ? errno_from_muxed( err )
		                             : errno_from_mac_error( err );
		
		p7::throw_errno( errnum );
	}
	
	return result;
	
#endif
}
