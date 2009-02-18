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
	
	
	namespace Private
	{
		
		inline void FixAsyncResult( OSErr& err, OSErr result )
		{
			// Yes.  This is for real.
			// Non-debug 68K code in System 7.6 has witnessed a memory address
			// placed in D0 following a call to PBGetCatInfoAsync().
			
		#ifdef __MWERKS__
			
			const bool debugging = __option( sym );
			
			if ( TARGET_CPU_68K  &&  !debugging )
			{
				err = result < 0 ? result : 0;
			}
			
		#endif
		}
		
	}
	
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
		
		Private::FixAsyncResult( err, pb.ioParam.ioResult );
		
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

