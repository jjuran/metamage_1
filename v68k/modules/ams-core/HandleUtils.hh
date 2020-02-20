/*
	HandleUtils.hh
	--------------
*/

#ifndef HANDLEUTILS_HH
#define HANDLEUTILS_HH

typedef char*  Ptr;
typedef char** Handle;

pascal long Munger_patch( Handle h, long i, Ptr p1, long n1, Ptr p2, long n2 );

short HandToHand_patch( char** h : __A0 );

short PtrToHand_patch( char* p : __A0, long size : __D0 );

short PtrToXHand_patch( const void* p : __A0, char** h : __A1, long n : __D0 );

short HandAndHand_patch( char** src : __A0, char** dest : __A1 );

short PtrAndHand_patch( const void* p : __A0, char** h : __A1, long n : __D0 );

#endif
