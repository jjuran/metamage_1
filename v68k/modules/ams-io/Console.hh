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

#define CIn_open   NULL
#define COut_open  NULL

short CIn_prime ( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 );
short COut_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 );

#define CIn_control   NULL
#define COut_control  NULL

short CIn_status( short trap_word : __D1, CntrlParam* : __A0, DCE* dce : __A1 );

#define COut_status  NULL

#define CIn_close   NULL
#define COut_close  NULL

#endif
