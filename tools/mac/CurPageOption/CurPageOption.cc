/*
	CurPageOption.cc
	----------------
*/

// Standard C
#include <stdio.h>


#pragma exceptions off


#define LOWMEM( addr, type )  (*(type*) (addr))

#define MemTop         LOWMEM( 0x0108, void* )
#define BufPtr         LOWMEM( 0x010C, void* )
#define SoundBase      LOWMEM( 0x0266, void* )
#define ScrnBase       LOWMEM( 0x0824, void* )
#define CurPageOption  LOWMEM( 0x0936, short )


int main( int argc, char** argv )
{
#if ! TARGET_API_MAC_CARBON
	
	printf( "MemTop:        %p\n", MemTop        );
	printf( "BufPtr:        %p\n", BufPtr        );
	printf( "ScrnBase:      %p\n", ScrnBase      );
	printf( "SoundBase:     %p\n", SoundBase     );
	printf( "CurPageOption: %d\n", CurPageOption );
	
#endif
	
	return 0;
}
