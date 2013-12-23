/*	=======
 *	pump.cc
 *	=======
 */

// Standard C++
#include <algorithm>

// POSIX
#include <unistd.h>

// vfs
#include "vfs/filehandle/functions/seek.hh"

// Genie
#include "Genie/api/breathe.hh"
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static ssize_t pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count, unsigned flags )
	{
		std::size_t bytes_pumped = 0;
		
		try
		{
			StreamHandle& input  = GetFileHandleWithCast< StreamHandle >( fd_in  );
			StreamHandle& output = GetFileHandleWithCast< StreamHandle >( fd_out );
			
			breathe( true );
			
			if ( off_in != NULL )
			{
				seek( input, *off_in, 0 );
			}
			
			if ( off_out != NULL )
			{
				seek( output, *off_out, 0 );
			}
			
			char buffer[ 4096 ];
			
			const size_t size = sizeof buffer;
			
			ssize_t n_read;
			
			while (( n_read = input.Read( buffer, count ? std::min( size, count ) : size ) ))
			{
				ssize_t n_written = output.Write( buffer, n_read );
				
				bytes_pumped += n_written;
				
				if ( count != 0 )
				{
					if ( (count -= n_written) == 0 )
					{
						break;
					}
				}
				
				if ( breathe( false ) )
				{
					break;
				}
			}
			
			if ( off_in != NULL )
			{
				seek( input, *off_in, 0 );
				
				*off_in += bytes_pumped;
			}
			
			if ( off_out != NULL )
			{
				seek( output, *off_out, 0 );
				
				*off_out += bytes_pumped;
			}
			
			return bytes_pumped;
		}
		catch ( ... )
		{
			set_errno_from_exception();
			
			if ( bytes_pumped == 0 )
			{
				return -1;
			}
		}
		
		return bytes_pumped;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( pump );
	
	#pragma force_active reset
	
}

