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

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	plus::string CFStringGetStdString( CFStringRef string )
	{
		CFIndex length = CFStringGetLength( string );
		
		plus::var_string result;
		
		result.resize( length );
		
		CFIndex usedBufLen = 0;
		
		CFIndex nConverted = CFStringGetBytes( string,
		                                       CFRangeMake( 0, length ),
		                                       kCFStringEncodingMacRoman,
		                                       '\0',
		                                       false,
		                                       (UInt8*) &result[0],
		                                       length,
		                                       &usedBufLen );
		
		if ( usedBufLen != length )
		{
			N::ThrowOSStatus( paramErr );
		}
		
		return result;
	}
	
}

