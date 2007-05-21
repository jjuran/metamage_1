/*
 *	AEBuild.h										Copyright ©1991,1992 Apple Computer, Inc.
 */

#pragma once										/* For THINK C users */

#ifndef __AEBUILD__
#define __AEBUILD__									/* For poor MPW users :) */

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif


#define aeBuild_SyntaxErr	12345			/* Let's get an Official OSErr code someday */

typedef enum{								/* Syntax Error Codes: */
	aeBuildSyntaxNoErr = 0,						/* (No error) */
	aeBuildSyntaxBadToken,						/* Illegal character */
	aeBuildSyntaxBadEOF,						/* Unexpected end of format string */
	aeBuildSyntaxNoEOF,							/* Unexpected extra stuff past end */
	aeBuildSyntaxBadNegative,					/* "-" not followed by digits */
	aeBuildSyntaxMissingQuote,					/* Missing close "'" */
	aeBuildSyntaxBadHex,						/* Non-digit in hex string */
	aeBuildSyntaxOddHex,						/* Odd # of hex digits */
	aeBuildSyntaxNoCloseHex,					/* Missing "È" */
	aeBuildSyntaxUncoercedHex,					/* Hex string must be coerced to a type */
	aeBuildSyntaxNoCloseString,					/* Missing "Ó" */
	aeBuildSyntaxBadDesc,						/* Illegal descriptor */
	aeBuildSyntaxBadData,						/* Bad data value inside (É) */
	aeBuildSyntaxNoCloseParen,					/* Missing ")" after data value */
	aeBuildSyntaxNoCloseBracket,				/* Expected "," or "]" */
	aeBuildSyntaxNoCloseBrace,					/* Expected "," or "}" */
	aeBuildSyntaxNoKey,							/* Missing keyword in record */
	aeBuildSyntaxNoColon,						/* Missing ":" after keyword in record */
	aeBuildSyntaxCoercedList,					/* Cannot coerce a list */
	aeBuildSyntaxUncoercedDoubleAt				/* "@@" substitution must be coerced */
} AEBuild_SyntaxErrType;


#ifdef __cplusplus
extern "C" {
#endif

// In all the "v..." functions, the "args" parameter is really a va_list.
// It's listed as void* here to avoid having to #include stdarg.h.

// Building a descriptor:

OSErr
	AEBuild(  AEDesc *dst, const char *src, ... ),
	vAEBuild( AEDesc *dst, const char *src, const void *args );

// Adding a parameter to an Apple event:

OSErr
	AEBuildParameters( AppleEvent *event, const char *format, ... ),
	vAEBuildParameters( AppleEvent *event, const char *format, const void *args );

// Building an entire Apple event:

OSErr
	AEBuildAppleEvent(	AEEventClass theClass, AEEventID theID,
						DescType addressType, const void *addressData, long addressLength,
						short returnID, long transactionID, AppleEvent *result,
						const char *paramsFmt, ... ),
	vAEBuildAppleEvent(	AEEventClass theClass, AEEventID theID,
						DescType addressType, const void *addressData, long addressLength,
						short returnID, long transactionID, AppleEvent *resultEvt,
						const char *paramsFmt, const void *args );

#ifdef __cplusplus
}
#endif

#endif
