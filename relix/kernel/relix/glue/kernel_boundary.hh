/*
	kernel_boundary.hh
	------------------
*/

#ifndef RELIX_GLUE_KERNELBOUNDARY_HH
#define RELIX_GLUE_KERNELBOUNDARY_HH


namespace relix
{
	
	extern "C" long enter_system_call( long syscall_number );
	
	extern "C" bool leave_system_call();
	
}

#endif
