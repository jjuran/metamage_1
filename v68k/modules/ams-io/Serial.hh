/*
	Serial.hh
	---------
*/

#ifndef SERIAL_HH
#define SERIAL_HH

struct CntrlParam;
struct DCtlEntry;
struct IOParam;

typedef DCtlEntry DCE;

short xIn_prime ( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 );
short xOut_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 );

short xIn_status( short trap_word : __D1, CntrlParam* : __A0, DCE* dce : __A1 );

#endif
