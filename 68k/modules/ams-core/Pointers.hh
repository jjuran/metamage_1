/*
	Pointers.hh
	-----------
*/

#ifndef POINTERS_HH
#define POINTERS_HH

extern bool native_alloc;

char* NewPtr_patch( unsigned long size : __D0, short trap_word : __D1 );

short DisposePtr_patch( char* p : __A0 );

long GetPtrSize_patch( char* p : __A0 );

short SetPtrSize_patch( char* p : __A0, long size : __D0 );

#endif
