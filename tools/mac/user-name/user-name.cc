/*
	user-name.cc
	------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// POSIX
#include <unistd.h>

#if TARGET_API_MAC_CARBON

// Standard C
#include <stdlib.h>

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#endif  // #if TARGET_API_MAC_CARBON

// chars
#include "conv/mac_utf8.hh"

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( int argc, char** argv )
{
#if TARGET_API_MAC_CARBON
	
	if ( &CSCopyMachineName != NULL )
	{
		const bool useShortName = false;
		
		if ( CFStringRef name = CSCopyUserName( useShortName ) )
		{
			const CFStringEncoding encoding = kCFStringEncodingUTF8;
			
			CFIndex length = CFStringGetLength( name );
			
			CFRange range = CFRangeMake( 0, length );
			
			CFIndex usedBufLen = 0;
			
			(void) CFStringGetBytes( name,
			                         range,
			                         encoding,
			                         '\0',
			                         false,
			                         NULL,
			                         0,
			                         &usedBufLen );
			
			char* p = (char*) alloca( usedBufLen + 1 );
			
			(void) CFStringGetBytes( name,
			                         range,
			                         encoding,
			                         '\0',
			                         false,
			                         (UInt8*) p,
			                         usedBufLen,
			                         &usedBufLen );
			
			p[ usedBufLen++ ] = '\n';
			
			CFRelease( name );
			
			return wrote( STDOUT_FILENO, p, usedBufLen ) ? 0 : 13;
		}
	}
	
#endif  // #if TARGET_API_MAC_CARBON
	
	unsigned char str[ 255 ];
	
	if ( mac::rsrc::get_STR_resource( str, -16096 ) )
	{
		unsigned char* p = str;
		
		unsigned len = *p++;
		
		char utf[ 255 * 3 + 1 ];  // worst case
		
		size_t n = conv::utf8_from_mac( utf, sizeof utf, (char*) p, len );
		
		utf[ n++ ] = '\n';
		
		return wrote( STDOUT_FILENO, utf, n ) ? 0 : 13;
	}
	
	return 1;
}
