/*
	async.hh
	--------
*/

#ifndef ASYNC_HH
#define ASYNC_HH


struct SndChannel;

typedef char* Ptr;
typedef short OSErr;

typedef unsigned long UFixed;

typedef void (*cpy)( void* : __A1, const void* : __A0, unsigned long : __D0 );

OSErr play_async( SndChannel* chan, Ptr p, long n, UFixed rate, cpy f, int x );

#endif
