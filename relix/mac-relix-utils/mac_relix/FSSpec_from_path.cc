/*
	FSSpec_from_path.cc
	-------------------
*/

#include "mac_relix/FSSpec_from_path.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// POSIX
#include <sys/stat.h>

// Standard C
#include <errno.h>

#ifdef __APPLE__

// Standard C++
#include <algorithm>

// chars
#include "conv/mac_utf8.hh"

// mac-file-utils
#include "mac_file/directory.hh"

#endif

// mac-relix-utils
#include "mac_relix/FSSpec_from_stat.hh"


#pragma exceptions off


namespace mac   {
namespace relix {

#if defined( __RELIX__ )  ||  defined( __APPLE__ )

using mac::sys::muxed_errno;
using mac::sys::muxed_OSStatus;

Error FSSpec_from_existing_path( const char* path, FSSpec& result )
{
#ifdef __RELIX__
	
	struct stat st;
	
	int nok = stat( path, &st );
	
	if ( ! nok )
	{
		nok = FSSpec_from_stat( st, result );
	}
	
	return nok ? muxed_errno( errno ) : Error();
	
#endif
	
#ifdef __APPLE__
	
	OSStatus err;
	FSRef ref;
	
	(err = FSPathMakeRef( (const UInt8*) path, &ref, NULL ))  ||
	(err = FSGetCatalogInfo( &ref, 0, 0, 0, &result, 0 ));
	
	return muxed_OSStatus( err );
	
#endif
}

Error FSSpec_from_optional_path( const char* path, FSSpec& result )
{
#ifdef __RELIX__
	
	struct stat st;
	
	int nok = stat( path, &st );
	
	if ( ! nok  ||  errno == ENOENT )
	{
		nok = FSSpec_from_stat( st, result );
	}
	
	return nok ? muxed_errno( errno ) : Error();
	
#endif
	
#ifdef __APPLE__
	
	OSStatus err = FSSpec_from_existing_path( path, result );
	
#if ! __LP64__
	
	if ( err == fnfErr )
	{
		size_t len = strlen( path );
		
		char* p = (char*) alloca( len + 1 );
		
		memcpy( p, path, len + 1 );
		
		path = p;
		
		const char* name;
		
		if ( (p = strrchr( p, '/' )) )
		{
			*p++ = '\0';
			
			name = p;
		}
		else
		{
			name = path;
			path = ".";
		}
		
		if ( *name )
		{
			using conv::mac_from_utf8_nothrow;
			
			using mac::types::VRefNum_DirID;
			
			Byte* q = result.name;
			
			len = strlen( name );
			
			size_t n = mac_from_utf8_nothrow( (char*) q + 1, 32, name, len );
			
			if ( n > 31 )
			{
				return (Error) bdNamErr;
			}
			
			*q++ = n;
			
			std::replace( q, q + n, ':', '/' );
			
			FSRef ref;
			Boolean isDir;
			VRefNum_DirID& vdir = *(VRefNum_DirID*) &result;
			
			(err = FSPathMakeRef( (const Byte*) path, &ref, &isDir ))  ||
			(vdir = mac::file::directory( ref ),
			 err = vdir.dirID < 0 ? vdir.dirID : noErr);
		}
	}
	
#endif  // #if ! __LP64__
	
	return (Error) err;
	
#endif  // #ifdef __APPLE__
}

#endif  // #if defined( __RELIX__ )  ||  defined( __APPLE__ )

}
}
