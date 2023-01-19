/*	=====================
 *	GetWorkstationName.cc
 *	=====================
 */

#include "Genie/Utilities/GetWorkstationName.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// more-libc
#include "more/string.h"

// mac-sys-utils
#include "mac_sys/has/native_Carbon.hh"

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"


#pragma exceptions off


static inline
unsigned min( unsigned a, unsigned b )
{
	return b < a ? b : a;
}

namespace Genie
{
	
	void GetWorkstationName( char* buffer, unsigned size )
	{
		const unsigned max_len = size - 1;
		
		if ( ! mac::sys::has_native_Carbon() )
		{
			Str255 str;
			
			if ( mac::rsrc::get_STR_resource( str, -16413 ) )
			{
				ConstStr255Param p = str;
				const unsigned   n = min( *p++, max_len );
				
				mempcpy( buffer, p, n );
				
				buffer[ n ] = '\0';
			}
		}
		else if ( CFStringRef name = CSCopyMachineName() )
		{
			const CFStringEncoding encoding = kCFStringEncodingMacRoman;
			
			CFIndex length = CFStringGetLength( name );
			CFRange range  = CFRangeMake( 0, length );
			
			CFIndex usedBufLen = 0;
			
			(void) CFStringGetBytes( name,
			                         range,
			                         encoding,
			                         '\0',
			                         false,
			                         (UInt8*) buffer,
			                         max_len,
			                         &usedBufLen );
			
			buffer[ usedBufLen ] = '\0';
			
			CFRelease( name );
		}
	}
	
}
