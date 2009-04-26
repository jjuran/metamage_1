/*	=======
 *	sync.cc
 *	=======
 */

// POSIX
#include "unistd.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static void Flush_Volume( N::FSVolumeRefNum vRefNum )
	{
		OSErr err = ::FlushVol( NULL, vRefNum );
	}
	
	static void FlushAll()
	{
		N::Volume_Container volumes( N::Volumes() );
		
		std::for_each( volumes.begin(),
		               volumes.end(),
		               std::ptr_fun( Flush_Volume ) );
	}
	
	
	static int fdatasync( int fd )
	{
		SystemCallFrame frame( "fdatasync" );
		
		try
		{
			GetFileHandleWithCast< StreamHandle >( fd ).Synchronize( false );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	static int fsync( int fd )
	{
		SystemCallFrame frame( "fsync" );
		
		try
		{
			GetFileHandleWithCast< StreamHandle >( fd ).Synchronize( true );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	static void sync()
	{
		SystemCallFrame frame( "sync" );
		
		try
		{
			FlushAll();
		}
		catch ( ... )
		{
		}
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( fdatasync );
	REGISTER_SYSTEM_CALL( fsync     );
	REGISTER_SYSTEM_CALL( sync      );
	
	#pragma force_active reset
	
}

