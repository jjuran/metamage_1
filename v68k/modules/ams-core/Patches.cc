/*
	Patches.cc
	----------
*/

#include "Patches.hh"


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;


typedef unsigned short trap_u16;

static inline
bool is_toolbox_trap_num( trap_u16 trapNum : __D0 )
{
	trapNum &= 0x01FF;
	
	return trapNum >= 0x50  &&  trapNum != 0x54  &&  trapNum != 0x57;
}

static
void*& find_trap( trap_u16 trapNum : __D0, trap_u16 trap_word : __D1 )
{
	void** table;
	short  mask;
	
	/*
		If bit 9 is set in the trap word, then bit 10 specifies which trap
		table to use.  Failing that, we have to guess, based on which traps
		existed in the 64K ROM.
		
		Thanks to Elliot Nunn for the guessing fallback logic (from mps).
	*/
	
	bool is_toolbox = trap_word & 0x0200 ? trap_word & 0x0400
	                : is_toolbox_trap_num( trapNum );
	
	if ( is_toolbox )
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
