// Nitrogen/OSStatus.hh
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_OSSTATUS_HH
#define NITROGEN_OSSTATUS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#ifndef NUCLEUS_ERRORCODE_HH
#include "nucleus/error_code.hh"
#endif

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OSSTATUS_HH
#include "Mac/Toolbox/Types/OSStatus.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


namespace Nitrogen
{
	
	using Mac::OSStatus;
	using Mac::ThrowOSStatus;
	
	#define DEFINE_OSSTATUS( c_name, new_name )  DEFINE_ERRORCODE( OSStatus, c_name, new_name )
	
   template < ::OSStatus error >
   inline void RegisterOSStatus()
     {
      ::nucleus::register_error_code< OSStatus, error >();
     }
	
	inline void HandleDestructionOSStatus( ::OSStatus err )
	{
		// ignore errors in destructors for now
	}
	
	::OSStatus ConvertTheExceptionToOSStatus( ::OSStatus defaultValue );
	
  }

namespace Nitrogen
{
	
	template < ::OSStatus status >
	struct OSStatus_EndOfEnumeration : public ::nucleus::error_code_end_of_enumeration< OSStatus, status >
	{
	};
	
}

#endif

