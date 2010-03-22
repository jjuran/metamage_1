// Nitrogen/Devices.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_DEVICES_HH
#define NITROGEN_DEVICES_HH

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// nucleus
#include "nucleus/errors_registered.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


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
			
			// FIXME:  According to Scott Boyd and Jim Luther[1], the result of
			// async calls to the File Manager and PPC Toolbox should be IGNORED
			// entirely, and ioResult alone should be checked, whereas success
			// of a Device Manager call should be determined first by checking
			// the result for noErr, and only then polling ioResult.
			// Unfortunately, some calls are used by *both* the File Manager and
			// Device Manager, so a correct fix is more involved than simply
			// changing the code here.
			
			// [1] http://www.mactech.com/articles/develop/issue_13/Luther.html
			
			// In the meantime, we'll leave this code as it's been, with the
			// exception of removing its dependency on non-debug mode.
			
			if ( TARGET_CPU_68K )
			{
				err = result < 0 ? result : 0;
			}
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
		OSErr err = ::PBWriteAsync( &pb );
		
		Private::FixAsyncResult( err, pb.ioParam.ioResult );
		
		ThrowOSStatus( err );
	}
	
}

#endif

