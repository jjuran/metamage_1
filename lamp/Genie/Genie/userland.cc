/*
	userland.cc
	-----------
*/

#include "Genie/userland.hh"


namespace Genie
{
	
#ifdef __MC68K__
	
	asm void call_signal_handler( void* handler : __a0, int signo : __d0 )
	{
		LINK	A6, #0    ; // create stack frame
		MOVE.L	A4,-(SP)  ; // save A4
		
		MOVE.L	D0,-(SP)  ; // push signo
		
		JSR		(A0)      ; // call handler
		
		MOVEA.L	4(SP),A4  ; // restore A4
		
		UNLK	A6
		
		RTS
	}
	
#endif
	
}

