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


namespace Genie
{
	
	static int access( const char* path, int mode )
	{
		SystemCallFrame frame( "access" );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( path, current );
			
			ResolveLinks_InPlace( file );
			
			if ( !file->Exists() )
			{
				return frame.SetErrno( EACCES );
			}
			
			// FIXME: check permissions
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int chmod_file( SystemCallFrame& frame, const char* path, mode_t mode )
	{
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( path, current );
			
			ResolveLinks_InPlace( file );
			
			file->ChangeMode( mode );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	static int fchmod( int fd, mode_t mode )
	{
		SystemCallFrame frame( "fchmod" );
		
		try
		{
			GetFileHandle( fd )->GetFile()->ChangeMode( mode );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int stat_file( SystemCallFrame& frame, const char* path, struct stat* sb, bool resolveLinks )
	{
		Breathe();
		
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( path, current );
			
			if ( resolveLinks )
			{
				ResolveLinks_InPlace( file );
			}
			
			file->Stat( *sb );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	static int chmod( const char* path, mode_t mode )
	{
		SystemCallFrame frame( "chmod" );
		
		return chmod_file( frame, path, mode );
	}
	
	
	static int lstat( const char* path, struct stat* sb )
	{
		SystemCallFrame frame( "lstat" );
		
		return stat_file( frame, path, sb, false );
	}
	
	
	static int stat( const char* path, struct stat* sb )
	{
		SystemCallFrame frame( "stat" );
		
		return stat_file( frame, path, sb, true );  // FIXME:  Resolve symlinks
	}
	
	
	static int fstat( int fd, struct stat* sb )
	{
		SystemCallFrame frame( "fstat" );
		
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		try
		{
			GetFileHandle( fd )->GetFile()->Stat( *sb );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( access );
	REGISTER_SYSTEM_CALL( fchmod );
	REGISTER_SYSTEM_CALL( chmod  );
	REGISTER_SYSTEM_CALL( lstat  );
	REGISTER_SYSTEM_CALL( stat   );
	REGISTER_SYSTEM_CALL( fstat  );
	
	#pragma force_active reset
	
}

