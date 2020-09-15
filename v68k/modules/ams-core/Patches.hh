/*
	Patches.hh
	----------
*/

#ifndef PATCHES_HH
#define PATCHES_HH

void* GetTrapAddress_patch( unsigned short trapNum : __D0 );

void SetTrapAddress_patch( unsigned short trapNum : __D0, void* addr : __A0 );

#endif
