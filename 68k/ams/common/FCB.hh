/*
	FCB.hh
	------
*/

#ifndef FCB_HH
#define FCB_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

struct VCB;
struct FCB
{
	long    fcbFlNum;   // file number
	SInt8   fcbMdRByt;  // flags
	SInt8   fcbTypByt;  // version number
	UInt16  fcbSBlk;    // first allocation block
	long    fcbEOF;     // logical EOF
	long    fcbPLen;    // physical EOF
	long    fcbCrPs;    // mark
	VCB*    fcbVPtr;    // VCB pointer
	Ptr     fcbBfAdr;   // buffer address
	short   fcbFlPos;   // for internal use
};

/*
	NOTE:  kFCBCount, and therefore the size of FCBS, are private to ams-fs,
	which is solely responsible for allocating and knowing the size of the
	FCB buffer.  It's possible that in the future, kFCBCount may be changed,
	or removed entirely in favor of dynamically sizing the buffer -- in which
	event it's expected that other clients will cope with such change without
	needing to be recompiled.
	
	That said, clients can rely on the format of the FCB structure, a minimum
	of 12 FCBs, and the buffer address in FCBSPtr.
	
	(This is intended for the benefit of ams-rsrc.)
*/

enum { kFCBCount = 48 };

struct FCBS
{
	UInt16  bufSize;
	FCB     fcbs[ kFCBCount ];
};

FCBS* FCBSPtr : 0x034E;

#endif
