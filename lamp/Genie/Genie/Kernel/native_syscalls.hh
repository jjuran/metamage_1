/*
	native_syscalls.hh
	------------------
*/

#ifndef GENIE_KERNEL_NATIVESYSCALLS_HH
#define GENIE_KERNEL_NATIVESYSCALLS_HH


namespace Genie
{
	
	int native_symlink( const char* target_path, const char* link_path );
	
	int native_unlink( const char* path );
	
}

#endif

