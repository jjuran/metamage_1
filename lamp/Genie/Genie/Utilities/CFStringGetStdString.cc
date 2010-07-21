/*	=======================
 *	CFStringGetStdString.cc
 *	=======================
 */

#include "Genie/Utilities/CFStringGetStdString.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// plus
#include "plus/var_string.hh"


namespace Genie
{
	
	plus::string CFStringGetStdString( CFStringRef       string,
	                                   CFStringEncoding  encoding )
	{
		CFIndex length = CFStringGetLength( string );
		
		CFRange range = CFRangeMake( 0, length );
		
		CFIndex usedBufLen = 0;
		
		(void) CFStringGetBytes( string,
		                         range,
		                         encoding,
		                         '\0',
		                         false,
		                         NULL,
		                         0,
		                         &usedBufLen );
		
		plus::var_string result;
		
		char* p = result.reset( usedBufLen );
		
		(void) CFStringGetBytes( string,
		                         range,
		                         encoding,
		                         '\0',
		                         false,
		                         (UInt8*) p,
		                         usedBufLen,
		                         &usedBufLen );
		
		return result;
	}
	
}

