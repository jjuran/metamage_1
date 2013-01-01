// AEGizmos.cc

// Mac OS
#ifndef __AEHELPERS__
#include <AEHelpers.h>
#endif

// AEGizmos
#ifndef __AEBUILD__
#include "AEBuild.h"
#include "AEBuildGlobals.h"
#endif

static AEBuildError MakeAEBuildError()
{
	AEBuildError error;
	
	error.fError    = AEBuild_ErrCode;
	error.fErrorPos = AEBuild_ErrPos;
	
	return error;
}

OSStatus vAEBuildAppleEvent( AEEventClass    theClass,
                             AEEventID       theID,
                             DescType        addressType,
                             const void *    addressData,
                             long            addressLength,
                             short           returnID,
                             long            transactionID,
                             AppleEvent *    resultEvt,
                             AEBuildError *  error,               /* can be NULL */
                             const char *    paramsFmt,
                             va_list         args )
{
	OSErr err = AEGizmos_vAEBuildAppleEvent( theClass,
	                                         theID,
	                                         addressType,
	                                         addressData,
	                                         addressLength,
	                                         returnID,
	                                         transactionID,
	                                         resultEvt,
	                                         //error,
	                                         paramsFmt,
	                                         args );
	
	if ( error != NULL )
	{
		*error = MakeAEBuildError();
	}
	
	return err;
}

OSStatus AEBuildDesc( AEDesc *        dst,
                      AEBuildError *  error,       /* can be NULL */
                      const char *    src,
                      ... )
{
	va_list args;
	va_start( args, src );
	
	OSErr err = AEGizmos_vAEBuild( dst, src, args );
	
	va_end( args );
	
	if ( error != NULL )
	{
		*error = MakeAEBuildError();
	}
	
	return err;
}

