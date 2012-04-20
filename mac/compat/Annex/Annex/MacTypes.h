/*
	Annex/MacTypes.h
	----------------
	
	Written in 2011 by Joshua Juran, who places it in the public domain.
*/

#ifndef ANNEX_MACTYPES_H
#define ANNEX_MACTYPES_H

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef MAC_OS_X_VERSION_10_5

typedef void* PRefCon;

#if __LP64__

typedef void* SRefCon;
typedef void* URefCon;

#else

typedef SInt32 SRefCon;
typedef UInt32 URefCon;

#endif  // #ifdef __LP64__

#endif  // #ifndef MAC_OS_X_VERSION_10_5

#endif

