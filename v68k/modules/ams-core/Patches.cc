/*
	Patches.cc
	----------
*/

#include "Patches.hh"


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;


typedef unsigned short trap_u16;

static
void*& find_trap( trap_u16 trapNum : __D0, trap_u16 trap_word : __D1 )
{
	void** table;
	short  mask;
	
	if ( trapNum & 0x0800 )
	{
		table = toolbox_trap_table;
		mask  = 0x03FF;
	}
	else
	{
		table = os_trap_table;
		mask  = 0x00FF;
	}
	
	return table[ trapNum & mask ];
}

void* GetTrapAddress_patch( short trapNum : __D0, short trap_word : __D1 )
{
	return find_trap( trapNum, trap_word );
}

void SetTrapAddress_patch( void* addr      : __A0,
                           short trapNum   : __D0,
                           short trap_word : __D1 )
{
	find_trap( trapNum, trap_word ) = addr;
}
