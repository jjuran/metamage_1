/*
	v68k-count.cc
	-------------
*/

// Standard C
#include <stdio.h>

// v68k
#include "v68k/decode.hh"
#include "v68k/instruction.hh"


#pragma exceptions off


static void count()
{
	int n_valid      = 0;
	int n_privileged = 0;
	int not_68000    = 0;
	
	for ( int i = 0;  i < 65536;  ++i )
	{
		v68k::instruction storage = { 0 };
		
		const v68k::instruction* decoded = v68k::decode( i, storage );
		
		if ( decoded )
		{
			++n_valid;
			
			if ( decoded->flags & v68k::privilege_mask )
			{
				++n_privileged;
			}
			
			if ( decoded->flags & v68k::not_before_mask )
			{
				++not_68000;
			}
		}
	}
	
	printf( "%d valid opcodes, %d privileged, %d not on 68000\n", n_valid, n_privileged, not_68000 );
}

int main( int argc, char** argv )
{
	count();
	
	return 0;
}
