/*
	native_syscalls.cc
	------------------
*/

#include "Genie/Kernel/native_syscalls.hh"

#include "Nitrogen/CFBundle.hh"
#include "Nitrogen/CFURL.hh"

// Standard C
#include <errno.h>


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static CFBundleRef get_System_bundle()
	{
		static CFBundleRef bundle = N::CFBundleCreate( N::CFURLCreateWithString( CFSTR( "/System/Library/Frameworks/System.framework" ) ) ).release();
		
		return bundle;
	}
	
	static void* get_native_syscall( CFStringRef syscall_name )
	{
		return N::CFBundleGetFunctionPointerForName( get_System_bundle(), syscall_name );
	}
	
	static int get_errno()
	{
		// <sys/errno.h> declares __error() and defines errno in terms of it
		
		typedef int* (*error_proc)();
		
		static error_proc _error = (error_proc) get_native_syscall( CFSTR( "__error" ) );
		
		return *_error();
	}
	
	int native_unlink( const char* path )
	{
		typedef int (*unlink_proc)( const char* );
		
		try
		{
			(void) get_errno();  // Make sure loading __error() doesn't clobber errno
			
			unlink_proc _unlink = (unlink_proc) get_native_syscall( CFSTR( "unlink" ) );
			
			int result = _unlink( path );
			
			if ( result < 0 )
			{
				errno = get_errno();
			}
			
			return result;
		}
		catch ( ... )
		{
		}
		
		errno = ENOSYS;
		
		return -1;
	}
	
}

