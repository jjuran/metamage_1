/*	=========
 *	divide.cc
 *	=========
 */

// POSIX
#include <fcntl.h>
#include <unistd.h>

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"

// gear
#include "gear/find.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/extras/pump.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
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
	
	static void decode_escapes( plus::var_string& decoded, const char* escaped_string )
	{
		const char* p = escaped_string;
		
		for ( const char* p = escaped_string;  *p != '\0';  ++p )
		{
			if ( *p != '\\' )
			{
				decoded += *p;
				
				continue;
			}
			
			const char* q = ++p;
			
			while ( iota::is_digit( *q )  &&  q - p < 3 )
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
				case '\0':  return;
				
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
	}
	
	int Main( int argc, char** argv )
	{
		// Check for sufficient number of args
		if ( argc < 4 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: divide <token> <outfile1> <outfile2>\n" ) );
			
			return 2;
		}
		
		const char* divider_text = argv[1];
		
		plus::var_string divider_string;
		
		decode_escapes( divider_string, divider_text );
		
		const char* outfile1 = argv[2];
		const char* outfile2 = argv[3];
		
		n::owned< p7::fd_t > out1 = p7::open( outfile1, p7::o_wronly | p7::o_trunc | p7::o_creat );
		n::owned< p7::fd_t > out2 = p7::open( outfile2, p7::o_wronly | p7::o_trunc | p7::o_creat );
		
		bool divided = false;
		
		plus::var_string first_blocks;
		
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
			
			const char* fb_data = first_blocks.data();
			
			size_t fb_size = first_blocks.size();
			size_t ds_size = divider_string.size();
			
			const char* div = gear::find_first_match( fb_data,
			                                          fb_size,
			                                          divider_string.data(),
			                                          ds_size );
			
			if ( div != NULL )
			{
				const char* stop = div + ds_size;
				
				divided = true;
				
				unsigned part_one_size = stop    - fb_data;
				unsigned part_two_size = fb_size - part_one_size;
				
				p7::write( out1, fb_data, part_one_size );
				p7::write( out2, stop,    part_two_size );
			}
		}
		
		p7::pump( p7::stdin_fileno, out2 );
		
		return 0;
	}

}
