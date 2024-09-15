/*
	trap-info.cc
	------------
*/

// Mac OS
#ifndef __PATCHES__
#include <Patches.h>
#endif

// Standard C
#include <stdio.h>


#pragma exceptions off


enum
{
	_DisposePtr    = 0xA01F,
	_Get1Resource  = 0xA81F,
	_GetCPixel     = 0xAA1F,
};

#if CALL_NOT_IN_CARBON

static inline
bool has_unified_trap_table()
{
	return GetOSTrapAddress     ( _DisposePtr   ) ==
	       GetToolboxTrapAddress( _Get1Resource );
}
static inline
bool has_expanded_toolbox_trap_table()
{
	return GetToolboxTrapAddress( _Get1Resource ) !=
	       GetToolboxTrapAddress( _GetCPixel    );
}

#endif

static
void print_trap_info()
{
#if CALL_NOT_IN_CARBON
	
	if ( has_unified_trap_table() )
	{
		printf( "%s\n", "1K unified trap table, 512 entries (compressed)" );
	}
	else
	{
		printf( "%s\n", "1K OS trap table, 256 entries" );
		
		int k = (1 + has_expanded_toolbox_trap_table()) * 2;
		
		printf( "%dK TB trap table, %d entries\n", k, 256 * k );
	}
	
#endif
}

int main( int argc, char** argv )
{
	print_trap_info();
	
	return 0;
}
