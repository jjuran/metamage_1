/*
	Console.hh
	----------
*/

#ifndef CONSOLE_HH
#define CONSOLE_HH

struct CntrlParam;
struct DCtlEntry;
struct IOParam;

typedef DCtlEntry DCE;

short CIn_prime ( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 );
short COut_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 );

short CIn_status( short trap_word : __D1, CntrlParam* : __A0, DCE* dce : __A1 );

#define COut_status  NULL

#endif
