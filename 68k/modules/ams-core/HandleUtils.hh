/*
	HandleUtils.hh
	--------------
*/

#ifndef HANDLEUTILS_HH
#define HANDLEUTILS_HH

typedef char*  Ptr;
typedef char** Handle;

pascal long Munger_patch( Handle h, long i, Ptr p1, long n1, Ptr p2, long n2 );

short HandToHand_patch( Handle h : __A0 );

short PtrToHand_patch( Ptr p : __A0, long size : __D0 );

short PtrToXHand_patch( Ptr p : __A0, Handle h : __A1, long n : __D0 );

short HandAndHand_patch( Handle src : __A0, Handle dest : __A1 );

short PtrAndHand_patch( Ptr p : __A0, Handle h : __A1, long n : __D0 );

#endif
