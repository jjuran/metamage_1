/*
	Patches.cc
	----------
*/

#include "Patches.hh"


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;


static
void*& find_trap( short trapNum : __D0 )
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

void* GetTrapAddress_patch( short trapNum : __D0 ) : __A0
{
	return find_trap( trapNum );
}

void SetTrapAddress_patch( short trapNum : __D0, void* addr : __A0 )
{
	find_trap( trapNum ) = addr;
}
