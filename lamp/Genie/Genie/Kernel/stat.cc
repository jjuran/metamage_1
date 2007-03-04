/*	=======
 *	stat.cc
 *	=======
 */

// Standard C/C++
#include <cstring>

// Standard C++
#include <string>

// POSIX
//#include "stdlib.h"
#include "sys/stat.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/File.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_stat )
	DEFINE_MODULE_INIT(Kernel_stat)
	
	static int chmod_file( const char* path, mode_t mode )
	{
		try
		{
			FSTreePtr current = CurrentProcess().GetCWD();
			
			FSTreePtr file = ResolvePathname( path, current );
			
			file->ChangeMode( mode );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	static int stat_file( const char* path, struct stat* sb, bool resolveLinks )
	{
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		try
		{
			FSTreePtr current = CurrentProcess().GetCWD();
			
			FSTreePtr file = ResolvePathname( path, current );
			
			if ( resolveLinks && file->IsLink() )
			{
				file = file->ResolveLink();
			}
			
			file->Stat( *sb );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	static int chmod( const char* path, mode_t mode )
	{
		return chmod_file( path, mode );
	}
	
	REGISTER_SYSTEM_CALL( chmod );
	
	static int lstat( const char* path, struct stat* sb )
	{
		return stat_file( path, sb, false );
	}
	
	REGISTER_SYSTEM_CALL( lstat );
	
	static int stat( const char* path, struct stat* sb )
	{
		return stat_file( path, sb, true );  // FIXME:  Resolve symlinks
	}
	
	REGISTER_SYSTEM_CALL( stat );
	
	static int fstat( int fd, struct stat* sb )
	{
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		try
		{
			GetFileHandle( fd )->GetFile()->Stat( *sb );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( fstat );
	
}

