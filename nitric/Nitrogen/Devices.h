// Nitrogen/Devices.h
// ------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_DEVICES_H
#define NITROGEN_DEVICES_H

// Mac OS
#include <Devices.h>

// Nucleus
#include "Nucleus/ErrorsRegistered.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( DeviceManager );
	
	
	// Mac OS semantics
	struct ThrowEOF_Always
	{
		static bool SuppressError( OSStatus err, bool bytesRead )
		{
			return false;
		}
	};
	
	struct ThrowEOF_OnZero
	{
		static bool SuppressError( OSStatus err, bool bytesRead )
		{
			return err == eofErr  &&  bytesRead;
		}
	};
	
	// POSIX semantics
	struct ThrowEOF_Never
	{
		static bool SuppressError( OSStatus err, bool bytesRead )
		{
			return err == eofErr;
		}
	};
	
	template < class Policy >
	inline void ThrowReadOSStatus( OSStatus err, bool bytesRead, Policy policy )
	{
		if ( !policy.SuppressError( err, bytesRead ) )
		{
			ThrowOSStatus( err );
		}
	}
	
	
	template < class EOF_Policy >
	inline void PBReadAsync( ParamBlockRec& pb, EOF_Policy policy )
	{
		NUCLEUS_REQUIRE_ERRORS( DeviceManager );
		
		OSErr err = ::PBReadAsync( &pb );
		
		if ( err != noErr )
		{
			ThrowReadOSStatus( err, pb.ioParam.ioActCount, policy );
			
			pb.ioParam.ioResult = noErr;
		}
	}
	
	
	inline void PBWriteAsync( ParamBlockRec& pb )
	{
		ThrowOSStatus( ::PBWriteAsync( &pb ) );
	}
	
}

#endif

