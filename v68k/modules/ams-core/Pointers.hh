/*
	Pointers.hh
	-----------
*/

#ifndef POINTERS_HH
#define POINTERS_HH

extern bool native_alloc;

char* NewPtr_patch( long size : __D0, short trap_word : __D1 ) : __A0;

short DisposePtr_patch( char* p : __A0 ) : __D0;

#endif
