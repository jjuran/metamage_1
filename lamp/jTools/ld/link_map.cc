/*
	link_map.cc
	-----------
	
	Copyright 2009, Joshua Juran
*/

#include "link_map.hh"

// Standard C++
#include <vector>

// iota
#include "iota/decimal.hh"
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/concat.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/write.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static std::vector< uint32_t > global_code_offsets;
	static std::vector< uint32_t > global_name_offsets;
	
	static plus::var_string global_name_data;
	
	
	static bool begins_with( const char* a, const char* b )
	{
		while ( *b )
		{
			if ( *a++ != *b++ )
			{
				return false;
			}
		}
		
		return true;
	}
	
	
	static uint32_t decode_24_bit_hex( const char* s )
	{
		using iota::decoded_hex_digit;
		
		const uint32_t x = decoded_hex_digit( s[ 0 ] ) << 20
		                 | decoded_hex_digit( s[ 1 ] ) << 16
		                 | decoded_hex_digit( s[ 2 ] ) << 12
		                 | decoded_hex_digit( s[ 3 ] ) <<  8
		                 | decoded_hex_digit( s[ 4 ] ) <<  4
		                 | decoded_hex_digit( s[ 5 ] ) <<  0;
		
		return x;
	}
	
	static void read_code_line( const plus::string& line )
	{
		const size_t offset_column = STRLEN( "Code: " );
		
		const size_t offset_field_size = STRLEN( "123456" );
		
		if ( line.length() < offset_column + offset_field_size )
		{
			return;
		}
		
		const uint32_t offset = decode_24_bit_hex( &line[ offset_column ] );
		
		global_code_offsets.push_back( offset );
		
		const char* p = line.c_str() + offset_column + offset_field_size;
		
		while ( *p == ' ' )
		{
			++p;
		}
		
		const size_t length = iota::parse_unsigned_decimal( p );
		
		const size_t name_column = STRLEN( "Code: 0079b2   12345 bytes  " ) + sizeof '"';
		
		const size_t name_end = line.find( '"', name_column + 1 );
		
		global_name_offsets.push_back( global_name_data.size() );
		
		global_name_data.append( &line[ name_column ], &line[ name_end ] );
		
		global_name_data += '\0';
	}
	
	static void read_link_map( p7::fd_t fd )
	{
		text_input::feed feed;
		
		p7::fd_reader reader( fd );
		
		while ( const plus::string* s = get_line_from_feed( feed, reader ) )
		{
			const plus::string& line = *s;
			
			if ( line.length() == 1 )
			{
				break;
			}
		}
		
		get_line_from_feed( feed, reader );  // Segment: 1
		
		while ( const plus::string* s = get_line_from_feed( feed, reader ) )
		{
			const plus::string& line = *s;
			
			if ( begins_with( line.c_str(), "Code:" ) )
			{
				read_code_line( line );
			}
			else
			{
				break;
			}
		}
	}
	
	
	uint32_t get_code_offset( const char* name )
	{
		const size_t n = global_name_offsets.size();
		
		for ( unsigned i = 0;  i < n;  ++i )
		{
			const uint32_t offset = global_name_offsets[ i ];
			
			if ( strcmp( name, &global_name_data[ offset ] ) == 0 )
			{
				return global_code_offsets[ i ];
			}
		}
		
		return 0;
	}
	
	void record_symbolics( const char* output_path, unsigned path_size )
	{
	#ifndef __LAMP__
		
		return;  // No path-based resource access
		
	#endif
		
		const plus::string linkmap_path = plus::concat( output_path, path_size, STR_LEN( ".map" ) );
		
		read_link_map( p7::open( linkmap_path, p7::o_rdonly ) );
	}
	
}

