/*
	Patches.hh
	----------
*/

#ifndef PATCHES_HH
#define PATCHES_HH

void* GetTrapAddress_patch( short trapNum : __D0 ) : __A0;

void SetTrapAddress_patch( short trapNum : __D0, void* addr : __A0 );

#endif
