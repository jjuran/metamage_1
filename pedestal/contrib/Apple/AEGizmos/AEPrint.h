/*
 *	AEPrint.h										Copyright ©1991-1993 Apple Computer, Inc.
 */

#pragma once

#ifndef __AEPRINT__
#define __AEPRINT__

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

OSErr AEPrint( AEDesc *desc, char *bufStr, long bufSize );
OSErr AEPrintSize( AEDesc *desc, long *stringLength );		// Just computes size (len+1) of string

#ifdef __cplusplus
}
#endif

#endif