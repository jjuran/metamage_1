/*
	unglue.hh
	---------
*/

#ifndef UNGLUE_HH
#define UNGLUE_HH


enum
{
	_SetHandleSize = 0xA024,
	
	_PtrToHand   = 0xA9E3,
	_HandAndHand = 0xA9E4,
};

typedef char**           Handle;
typedef unsigned char**  StringHandle;
typedef unsigned long    UInt32;

inline asm
StringHandle PtrToHand( const void* p : __A0, UInt32 n : __D0 )
{
	_PtrToHand
}

inline asm
Handle CopyHandle( Handle src : __A1, Handle dst : __A0 )
{
	MOVEQ.L  #0,D0
	_SetHandleSize
	
	EXG      A0,A1
	_HandAndHand
	
	// dst is now in A0
}

#endif
