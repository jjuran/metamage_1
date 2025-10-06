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

OSErr play_async( SndChannel* chan, Ptr p, long n_samples, UFixed rate );

#endif
