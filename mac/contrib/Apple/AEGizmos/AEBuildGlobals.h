/*
 *	AEBuildGlobals.h								Copyright ©1991 Apple Computer, Inc.
 */

#ifndef __AEBUILD__
#include <AEBuild.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern AEBuild_SyntaxErrType
	AEBuild_ErrCode;					/* Examine after AEBuild returns a syntax error */
extern long
	AEBuild_ErrPos;						/* Index of error in format string */

#ifdef __cplusplus
}
#endif

