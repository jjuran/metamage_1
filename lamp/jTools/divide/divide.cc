/*	=========
 *	divide.cc
 *	=========
 */

// POSIX
#include "fcntl.h"
#include "unistd.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;


inline const char* find_byte( const char* begin, const char* end, char c )
{
	const char* p = std::find( begin, end, c );
	
	if ( p == end )
	{
		return NULL;
	}
	
	return p;
}

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

static char decode_octal_byte( const char* begin, const char* end )
{
	int result = 0;
	
	while ( begin < end )
	{
		result = (result * 8) + (*begin - '0');
	}
	
	return result;
}

static std::string decode_escapes( const std::string& escaped_string )
{
	std::string decoded;
	
	for ( const char* p = escaped_string.c_str();  p < escaped_string.end();  ++p )
	{
		if ( *p != '\\' )
		{
			decoded += *p;
			
			continue;
		}
		
		const char* q = p;
		
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
		
		char c = *++p;
		
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

int O::Main( int argc, const char *const argv[] )
{
	// Check for sufficient number of args
	if ( argc < 4 )
	{
		Io::Err << "divide: Usage: divide <token> <outfile1> <outfile2>\n";
		
		return 1;
	}
	
	std::string divider_text = argv[1];
	
	std::string divider_string = decode_escapes( divider_text );
	
	const char* outfile1 = argv[2];
	const char* outfile2 = argv[3];
	
	NN::Owned< P7::FileDescriptor > out1 = P7::Open( outfile1, O_WRONLY | O_TRUNC | O_CREAT, 0644 );
	NN::Owned< P7::FileDescriptor > out2 = P7::Open( outfile2, O_WRONLY | O_TRUNC | O_CREAT, 0644 );
	
	bool divided = false;
	
	std::string first_blocks;
	
	enum { blockSize = 4096 };
	
	char data[ blockSize ];
	
	while ( !divided )
	{
		try
		{
			int bytes = io::read( P7::kStdIn_FileNo, data, blockSize );
			
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
				
				io::write( out1, first_blocks.data(), part_one_size );
				io::write( out2, stop,                part_two_size );
			}
		}
		catch ( const io::end_of_input& err )
		{
			return 2;
		}
	}
	
	while ( true )
	{
		try
		{
			int bytes = io::read( P7::kStdIn_FileNo, data, blockSize );
			
			io::write( out2, data, bytes );
		}
		catch ( const io::end_of_input& err )
		{
			break;
		}
	}
	
	return 0;
}

