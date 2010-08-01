/*
 *	AEBuild.h										Copyright ©1991,1992 Apple Computer, Inc.
 */

#pragma once										/* For THINK C users */

#ifndef __AEBUILD__
#define __AEBUILD__									/* For poor MPW users :) */

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifndef __AEHELPERS__
#include <AEHelpers.h>
#endif

#define aeBuild_SyntaxErr	12345			/* Let's get an Official OSErr code someday */

typedef OSErr AEBuild_SyntaxErrType;


#ifdef __cplusplus
extern "C" {
#endif

// In all the "v..." functions, the "args" parameter is really a va_list.
// It's listed as void* here to avoid having to #include stdarg.h.

// Building a descriptor:

OSErr
	AEGizmos_AEBuild(  AEDesc *dst, const char *src, ... ),
	AEGizmos_vAEBuild( AEDesc *dst, const char *src, const void *args );

// Adding a parameter to an Apple event:

OSErr
	AEGizmos_AEBuildParameters( AppleEvent *event, const char *format, ... ),
	AEGizmos_vAEBuildParameters( AppleEvent *event, const char *format, const void *args );

// Building an entire Apple event:

OSErr
	AEGizmos_AEBuildAppleEvent(	AEEventClass theClass, AEEventID theID,
						DescType addressType, const void *addressData, long addressLength,
						short returnID, long transactionID, AppleEvent *result,
						const char *paramsFmt, ... ),
	AEGizmos_vAEBuildAppleEvent(	AEEventClass theClass, AEEventID theID,
						DescType addressType, const void *addressData, long addressLength,
						short returnID, long transactionID, AppleEvent *resultEvt,
						const char *paramsFmt, const void *args );

#ifdef __cplusplus
}
#endif

#endif
