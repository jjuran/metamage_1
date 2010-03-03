/*	=======================
 *	CFStringGetStdString.cc
 *	=======================
 */

#include "Genie/Utilities/CFStringGetStdString.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	std::string CFStringGetStdString( CFStringRef string )
	{
		CFIndex length = CFStringGetLength( string );
		
		std::string result;
		
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

