/*
	init_thread.hh
	--------------
*/

#include "mac_sys/init_thread.hh"


namespace mac {
namespace sys {
	
#if defined( __MWERKS__ )  &&  defined( __MACOS__ )

	/*
		init_thread()
		
		Return the stack frame pointer.
		
		On PPC CFM, zero the backlink since Mac OS didn't bother.
		Otherwise, stack crawls will run off the end of the stack.
	*/
	
#ifdef __POWERPC__
	
	asm void* init_thread()
	{
		lwz r4,0(sp)
		li  r0,0
		lwz r3,0(r4)
		stw r0,0(r3)
	}
	
#else
	
	asm void* init_thread()
	{
		MOVEA.L (A6),A0
		RTS
	}
	
#endif  // #ifdef __POWERPC__

#else
	
	void* init_thread()
	{
		return 0;  // NULL
	}
	
#endif  // #if defined( __MWERKS__ )  &&  defined( __MACOS__ )
	
}
}
