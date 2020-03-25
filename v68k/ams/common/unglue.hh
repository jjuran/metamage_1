/*
	unglue.hh
	---------
*/

#ifndef UNGLUE_HH
#define UNGLUE_HH


enum
{
	_PtrToHand = 0xA9E3,
};

typedef unsigned char**  StringHandle;
typedef unsigned long    UInt32;

inline asm
StringHandle PtrToHand( const void* p : __A0, UInt32 n : __D0 )
{
	_PtrToHand
}

#endif
