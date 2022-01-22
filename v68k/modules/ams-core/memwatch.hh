/*
	memwatch.hh
	-----------
*/

#ifndef MEMWATCH_HH
#define MEMWATCH_HH

void WaitUntilZero_patch( unsigned short       trap : __D1,
                          const volatile char* addr : __A0 );

#endif
