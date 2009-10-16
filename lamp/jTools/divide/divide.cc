/*	=========
 *	divide.cc
 *	=========
 */

// Standard C/C++
#include <cctype>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/extras/pump.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static const char* FindSequenceInMemory( const char* mem_begin,
	                                         const char* mem_end,
	                                         const char* seq_begin,
	                                         const char* seq_end )
	{
		std::ptrdiff_t sequence_length = seq_end - seq_begin;
		
		while ( sequence_length <= mem_end - mem_begin )
		{
			if ( std::equal( seq_begin, seq_end, mem_begin ) )
			{
				return mem_begin;
			}
			
			++mem_begin;
		}
		
		return mem_end;
	}
	
	template < class Iter >
	char decode_octal_byte( Iter begin, Iter end )
	{
		int result = 0;
		
		while ( begin < end )
		{
			result = (result * 8) + (*begin++ - '0');
		}
		
		return result;
	}
	
	static std::string decode_escapes( const std::string& escaped_string )
	{
		std::string decoded;
		
		for ( std::string::const_iterator p = escaped_string.begin();  p < escaped_string.end();  ++p )
		{
			if ( *p != '\\' )
			{
				decoded += *p;
				
				continue;
			}
			
			std::string::const_iterator q = ++p;
			
			while ( std::isdigit( *q )  &&  q - p < 3 )
			{
				++q;
			}
			
			if ( q > p )
			{
				char c = decode_octal_byte( p, q );
				
				decoded += c;
				
				p = q - 1;
				
				continue;
			}
			
			char c = *p;
			
			switch ( c )
			{
				case '\0':  return decoded;
				
				case 'a':  c = '\a';  break;  // 0x07
				case 'b':  c = '\b';  break;  // 0x08
				case 't':  c = '\t';  break;  // 0x09
				case 'n':  c = '\n';  break;  // 0x0a
				case 'v':  c = '\v';  break;  // 0x0b
				case 'f':  c = '\f';  break;  // 0x0c
				case 'r':  c = '\r';  break;  // 0x0d
				
				default:  break;
			}
			
			decoded += c;
		}
		
		return decoded;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		// Check for sufficient number of args
		if ( argc < 4 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: divide <token> <outfile1> <outfile2>\n" ) );
			
			return 2;
		}
		
		std::string divider_text = argv[1];
		
		std::string divider_string = decode_escapes( divider_text );
		
		const char* outfile1 = argv[2];
		const char* outfile2 = argv[3];
		
		NN::Owned< p7::fd_t > out1 = p7::open( outfile1, p7::o_wronly | p7::o_trunc | p7::o_creat );
		NN::Owned< p7::fd_t > out2 = p7::open( outfile2, p7::o_wronly | p7::o_trunc | p7::o_creat );
		
		bool divided = false;
		
		std::string first_blocks;
		
		while ( !divided )
		{
			const std::size_t blockSize = 4096;
			
			char data[ blockSize ];
			
			std::size_t bytes = p7::read( p7::stdin_fileno, data, blockSize );
			
			if ( bytes == 0 )
			{
				// Divider token not found
				return 2;
			}
			
			first_blocks.append( data, data + bytes );
			
			const char* div = FindSequenceInMemory( &*first_blocks.begin(),
			                                        &*first_blocks.end(),
			                                        &*divider_string.begin(),
			                                        &*divider_string.end() );
			
			if ( div != data + bytes )
			{
				const char* stop = div + divider_string.size();
				
				divided = true;
				
				unsigned part_one_size = stop - first_blocks.data();
				unsigned part_two_size = first_blocks.size() - part_one_size;
				
				p7::write( out1, first_blocks.data(), part_one_size );
				p7::write( out2, stop,                part_two_size );
			}
		}
		
		p7::pump( p7::stdin_fileno, out2 );
		
		return 0;
	}

}

