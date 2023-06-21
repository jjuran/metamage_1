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

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_5
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif

// nucleus
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif

// Nitrogen
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( DeviceManager );
	
	
	/*
		According to Scott Boyd and Jim Luther[1], the result of async
		calls to the File Manager and PPC Toolbox should be IGNORED
		entirely, and ioResult alone should be checked, whereas success of
		a Device Manager call should be determined first by checking the
		result for noErr, and only then polling ioResult.  Unfortunately,
		some calls are used by *both* the File Manager and Device Manager,
		so we distinguish by checking the ioFRefNum field:  Negative values
		are device drivers and non-negative ones are files.
		
		[1] http://www.mactech.com/articles/develop/issue_13/Luther.html
		
		The necessity of substituting the ioResult value for the result of
		PBGetCatInfoAsync() has been confirmed for non-debug 68K code in
		System 7.6 -- a memory address is placed in D0.  The problem has
		not been witnessed in PPC code and almost certainly doesn't exist.
	*/
	
#if ! __LP64__
	
	inline
	void ThrowIOResult( OSErr err )
	{
		if ( err < 0 )
		{
			Mac::ThrowOSStatus_Internal( err );
		}
	}
	
	inline OSErr FixedAsyncResult( OSErr err, const ParamBlockRec& pb )
	{
		if ( TARGET_CPU_68K )
		{
			// Device drivers have negative refnums; files don't
			if ( const bool isFile = pb.ioParam.ioRefNum >= 0 )
			{
				return pb.ioParam.ioResult;
			}
		}
		
		return err;
	}
	
#endif  // #if ! __LP64__
	
	
	// Mac OS semantics
	struct ThrowEOF_Always
	{
		static bool SuppressError( ::OSStatus err, bool bytesRead )
		{
			return false;
		}
	};
	
	struct ThrowEOF_OnZero
	{
		static bool SuppressError( ::OSStatus err, bool bytesRead )
		{
			return err == eofErr  &&  bytesRead;
		}
	};
	
	// POSIX semantics
	struct ThrowEOF_Never
	{
		static bool SuppressError( ::OSStatus err, bool bytesRead )
		{
			return err == eofErr;
		}
	};
	
	template < class Policy >
	inline void ThrowReadOSStatus( ::OSStatus err, bool bytesRead, Policy policy )
	{
		if ( !policy.SuppressError( err, bytesRead ) )
		{
			ThrowIOResult( err );
		}
	}
	
	
#if ! __LP64__
	
	template < class EOF_Policy >
	inline void PBReadAsync( ParamBlockRec& pb, EOF_Policy policy )
	{
		NUCLEUS_REQUIRE_ERRORS( DeviceManager );
		
		if ( const OSErr err = FixedAsyncResult( ::PBReadAsync( &pb ), pb ) )
		{
			ThrowReadOSStatus( err, pb.ioParam.ioActCount, policy );
			
			pb.ioParam.ioResult = noErr;
		}
	}
	
	
	inline void PBWriteAsync( ParamBlockRec& pb )
	{
		ThrowIOResult( FixedAsyncResult( ::PBWriteAsync( &pb ), pb ) );
	}
	
#endif  // #if ! __LP64__
	
}

#endif
