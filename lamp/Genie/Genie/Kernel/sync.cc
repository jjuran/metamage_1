/*	=======
 *	sync.cc
 *	=======
 */

// POSIX
#include "unistd.h"

// Nitrogen
#include "Nitrogen/Files.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/SystemCallRegistry.hh"
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
		try
		{
			GetFileHandleWithCast< StreamHandle >( fd ).Synchronize( false );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	static int fsync( int fd )
	{
		try
		{
			GetFileHandleWithCast< StreamHandle >( fd ).Synchronize( true );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	static void sync()
	{
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

