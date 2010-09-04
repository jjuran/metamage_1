/*
	freeze.hh
	---------
*/

#ifndef PLUS_FREEZE_HH
#define PLUS_FREEZE_HH

// Standard C/C++
#include <cstring>

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	struct freeze_pod
	{
		template < class POD >
		static const char* get_data( const POD& pod )
		{
			return (const char*) &pod;  // reinterpret_cast
		}
		
		template < class POD >
		static std::size_t get_size( const POD& pod )
		{
			return sizeof pod;
		}
		
		template < class POD >
		static void apply( var_string& out, const POD& pod )
		{
			out.append( get_data( pod ), sizeof pod );
		}
	};
	
	/*
		freeze_c_string and freeze_pascal_string store the entire data
		structure, including the trailing NUL or leading length byte.
		Usually you don't want this, in which case you should specify e.g.
		stringify_pascal_string as freeze in addition to stringify.
	*/
	
	struct freeze_c_string
	{
		static const char* get_data( const char* s )
		{
			return s;
		}
		
		static std::size_t get_size( const char* s )
		{
			// Freeze the whole object, including trailing NUL byte
			return std::strlen( s ) + 1;
		}
		
		static void apply( var_string& out, const char* s )
		{
			out.append( get_data( s ), get_size( s ) );
		}
	};
	
	struct freeze_pascal_string
	{
		static const char* get_data( const unsigned char* s )
		{
			// Freeze the whole object, including length byte
			return (const char*) s;  // reinterpret_cast
		}
		
		static std::size_t get_size( const unsigned char* s )
		{
			// Freeze the whole object, including length byte
			return 1 + s[ 0 ];
		}
		
		static void apply( var_string& out, const unsigned char* s )
		{
			out.append( get_data( s ), get_size( s ) );
		}
	};
	
}

#endif

