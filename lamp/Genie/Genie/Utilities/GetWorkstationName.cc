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

// Nitrogen
#include "Nitrogen/CFBase.hh"

// Genie
#include "Genie/Utilities/CFStringGetStdString.hh"
#include "Genie/Utilities/GetStringResource.hh"


namespace Genie
{
	
	namespace n = nucleus;
	
	
	static const void* kUnresolvedCFragSymbolAddress = NULL;
	
	
	plus::string GetWorkstationName()
	{
		if ( ! mac::sys::has_native_Carbon() )
		{
			plus::string result = GetStringResource( -16413 );
			
			return result;
		}
		
		if ( CSCopyMachineName != kUnresolvedCFragSymbolAddress )
		{
			if ( CFStringRef name = CSCopyMachineName() )
			{
				CFStringEncoding encoding = kCFStringEncodingMacRoman;
				
				return CFStringGetStdString( n::owned< CFStringRef >::seize( name ), encoding );
			}
		}
		
		return plus::string::null;
	}
	
}
