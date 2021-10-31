/*
	Patches.hh
	----------
*/

#ifndef PATCHES_HH
#define PATCHES_HH

void* GetTrapAddress_patch( short trapNum : __D0, short trap_word : __D1 );

void SetTrapAddress_patch( void* addr      : __A0,
                           short trapNum   : __D0,
                           short trap_word : __D1 );

#endif
