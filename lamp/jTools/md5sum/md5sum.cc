/*	=========
 *	md5sum.cc
 *	=========
 */

// Standard C++
#include <algorithm>
#include <string>

// Standard C
#include <string.h>

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Arcana
#include "MD5.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	namespace Bits = BitsAndBytes;
	
	
	static ssize_t buffered_read( p7::fd_t fd, char* small_buffer, size_t n_bytes_requested )
	{
		static char big_buffer[ 4096 ];
		
		static size_t mark = 0;
		static size_t n_bytes_available = 0;
		
		if ( n_bytes_available == 0 )
		{
			n_bytes_available = p7::read( fd, big_buffer, sizeof big_buffer );
			
			mark = 0;
		}
		
		const size_t bytes_to_copy = std::min( n_bytes_requested, n_bytes_available );
		
		memcpy( small_buffer, big_buffer + mark, bytes_to_copy );
		
		n_bytes_available -= bytes_to_copy;
		mark              += bytes_to_copy;
		
		return bytes_to_copy;
	}
	
	static std::string MD5Sum( p7::fd_t input )
	{
		const std::size_t blockSize = 64;
		
		char data[ blockSize ];
		std::size_t bytes;
		MD5::Engine engine;
		
		// loop exits on a partial block or at EOF
		while ( ( bytes = buffered_read( input, data, blockSize ) ) == blockSize )
		{
			engine.DoBlock( data );
		}
		
		engine.Finish( data, bytes * 8 );
		
		std::string digest = Bits::EncodeAsHex( engine.GetResult() );
		
		return digest;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		int fail = 0;
		
		for ( int i = 1;  i < argc;  ++i )
		{
			try
			{
				std::printf( "%s  %s\n",
				              MD5Sum( p7::open( argv[ i ], p7::o_rdonly ) ).c_str(),
				                  argv[ i ] );
			}
			catch ( ... )
			{
				fail++;
			}
		}
		
		return fail == 0 ? 0 : 1;
	}

}

