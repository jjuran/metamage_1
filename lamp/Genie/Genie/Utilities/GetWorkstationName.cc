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

// mac-sys-utils
#include "mac_sys/has/native_Carbon.hh"

// plus
#include "plus/mac_utf8.hh"

// Nitrogen
#include "Nitrogen/CFBase.hh"

// Genie
#include "Genie/Utilities/CFStringGetStdString.hh"
#include "Genie/Utilities/GetStringResource.hh"


namespace Genie
{
	
	namespace n = nucleus;
	
	
	static const void* kUnresolvedCFragSymbolAddress = NULL;
	
	
	plus::string GetWorkstationName( bool convert_to_UTF8 )
	{
		if ( ! mac::sys::has_native_Carbon() )
		{
			plus::string result = GetStringResource( -16413 );
			
			if ( convert_to_UTF8 )
			{
				result = plus::utf8_from_mac( result );
			}
			
			return result;
		}
		
		if ( CSCopyMachineName != kUnresolvedCFragSymbolAddress )
		{
			if ( CFStringRef name = CSCopyMachineName() )
			{
				CFStringEncoding encoding = convert_to_UTF8 ? kCFStringEncodingUTF8
				                                            : kCFStringEncodingMacRoman;
				
				return CFStringGetStdString( n::owned< CFStringRef >::seize( name ), encoding );
			}
		}
		
		return plus::string::null;
	}
	
}
