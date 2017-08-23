/*
	utable.cc
	---------
*/

// Mac OS
#ifndef __APPLE__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#endif

// Standard C
#include <stdio.h>


static void** const UTableBase_Ptr          = (void**) 0x011c;
static short* const UnitTableEntryCount_Ptr = (short*) 0x01d2;

#define TVECTOR( name )  &name, #name
#define ENTRYPT( name )  *((void**) &name), #name
#define GLOBAL( name )   *name ## _Ptr, #name
#define LMGET( name )  LMGet ## name(), #name

int main( int argc, char** argv )
{
	printf( "%s\n", "T-Vector addresses:" );
	printf( "    %p: %s\n", TVECTOR( LMGetUTableBase ) );
	printf( "    %p: %s\n", TVECTOR( LMGetUnitTableEntryCount ) );
	
	printf( "%s\n", "function addresses:" );
	printf( "    %p: %s\n", ENTRYPT( LMGetUTableBase ) );
	printf( "    %p: %s\n", ENTRYPT( LMGetUnitTableEntryCount ) );
	
	printf( "%s\n", "low memory globals:" );
	printf( "    %p: %s\n", GLOBAL( UTableBase ) );
	printf( "    %d: %s\n", GLOBAL( UnitTableEntryCount ) );
	
	printf( "%s\n", "call results:" );
	printf( "    %p: %s\n", LMGET( UTableBase ) );
	printf( "    %d: %s\n", LMGET( UnitTableEntryCount ) );
	
	return 0;
}
