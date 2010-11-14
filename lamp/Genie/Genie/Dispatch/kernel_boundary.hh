/*
	kernel_boundary.hh
	------------------
*/

#ifndef GENIE_DISPATCH_KERNELBOUNDARY_HH
#define GENIE_DISPATCH_KERNELBOUNDARY_HH


namespace Genie
{
	
	extern "C" void enter_system_call( long syscall_number, long* params );
	
	extern "C" void leave_system_call( int result );
	
}

#endif

