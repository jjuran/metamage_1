/*
	native_syscalls.cc
	------------------
*/

#include "Genie/Kernel/native_syscalls.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// Mac OS
#ifndef __COREFOUNDATION_CFBUNDLE__
#ifndef __CFBUNDLE__
#include <CFBundle.h>
#endif
#endif
#ifndef __COREFOUNDATION_CFURL__
#ifndef __CFURL__
#include <CFURL.h>
#endif
#endif

// Standard C
#include <errno.h>


#pragma exceptions off


#define SYSTEM_FRAMEWORK  "/System/Library/Frameworks/System.framework"


namespace Genie
{
	
	static inline
	CFURLRef CFURLCreateWithString( CFStringRef string )
	{
		return CFURLCreateWithString( kCFAllocatorDefault, string, NULL );
	}
	
	static CFBundleRef get_System_bundle()
	{
		static CFStringRef str = CFSTR( SYSTEM_FRAMEWORK );
		static CFURLRef    url = CFURLCreateWithString( str );
		
		static CFBundleRef bundle = CFBundleCreate( kCFAllocatorDefault, url );
		
		return bundle;
	}
	
	static void* get_native_syscall( CFStringRef syscall_name )
	{
		return CFBundleGetFunctionPointerForName( get_System_bundle(), syscall_name );
	}
	
	static int get_errno()
	{
		// <sys/errno.h> declares __error() and defines errno in terms of it
		
		typedef int* (*error_proc)();
		
		static error_proc _error = (error_proc) get_native_syscall( CFSTR( "__error" ) );
		
		if ( _error == NULL )
		{
			return ENOSYS;
		}
		
		return *_error();
	}
	
	int native_symlink( const char* target_path, const char* link_path )
	{
		typedef int (*symlink_proc)( const char*, const char* );
		
		(void) get_errno();  // Make sure loading __error() doesn't clobber errno
		
		symlink_proc _symlink = (symlink_proc) get_native_syscall( CFSTR( "symlink" ) );
		
		if ( _symlink )
		{
			int result = _symlink( target_path, link_path );
			
			if ( result < 0 )
			{
				errno = get_errno();
			}
			
			return result;
		}
		
		errno = ENOSYS;
		
		return -1;
	}
	
	int native_unlink( const char* path )
	{
		typedef int (*unlink_proc)( const char* );
		
		(void) get_errno();  // Make sure loading __error() doesn't clobber errno
		
		unlink_proc _unlink = (unlink_proc) get_native_syscall( CFSTR( "unlink" ) );
		
		if ( _unlink )
		{
			int result = _unlink( path );
			
			if ( result < 0 )
			{
				errno = get_errno();
			}
			
			return result;
		}
		
		errno = ENOSYS;
		
		return -1;
	}
	
}
