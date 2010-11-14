/*	=======
 *	pump.cc
 *	=======
 */

// POSIX
#include <unistd.h>

// Debug
#include "debug/assert.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static ssize_t pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count, unsigned flags )
	{
		SystemCallFrame frame( "pump" );
		
		if ( count == 0 )
		{
			--count;
		}
		
		std::size_t bytes_pumped = 0;
		
		try
		{
			StreamHandle& input  = GetFileHandleWithCast< StreamHandle >( fd_in  );
			StreamHandle& output = GetFileHandleWithCast< StreamHandle >( fd_out );
			
			if ( off_in != NULL )
			{
				IOHandle_Cast< RegularFileHandle >( input ).Seek( *off_in, 0 );
			}
			
			if ( off_out != NULL )
			{
				IOHandle_Cast< RegularFileHandle >( output ).Seek( *off_out, 0 );
			}
			
			while ( const plus::string* peek_buffer = input.Peek( 1 ) )
			{
				Breathe();
				
				size_t bytes_wanted = std::min( count - bytes_pumped, peek_buffer->size() );
				
				int bytes_written = output.Write( peek_buffer->data(), bytes_wanted );
				
				input.Read( NULL, bytes_written );
				
				bytes_pumped += bytes_written;
				
				if ( bytes_pumped == count )
				{
					break;
				}
				
				ASSERT( bytes_pumped < count );
			}
			
			if ( off_in != NULL )
			{
				IOHandle_Cast< RegularFileHandle >( input ).Seek( *off_in, 0 );
				
				*off_in += bytes_pumped;
			}
			
			if ( off_out != NULL )
			{
				IOHandle_Cast< RegularFileHandle >( output ).Seek( *off_out, 0 );
				
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

