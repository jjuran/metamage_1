/*
	Console.hh
	----------
*/

#ifndef CONSOLE_HH
#define CONSOLE_HH

struct CntrlParam;
struct IOParam;

short CIn_prime ( short trap_word : __D1, IOParam* pb : __A0 );
short COut_prime( short trap_word : __D1, IOParam* pb : __A0 );

short CIn_status( short trap_word : __D1, CntrlParam* pb : __A0 );

#define COut_status  NULL

#endif
