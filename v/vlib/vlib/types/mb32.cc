/*
	mb32.cc
	-------
*/

#include "vlib/types/mb32.hh"

// more-libc
#include "more/string.h"

// chars
#include "charsets/extended_ascii.hh"

// plus
#include "plus/mac_utf8.hh"

// vlib
#include "vlib/quote.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"


#undef mempcpy


namespace vlib
{
	
	using ::mempcpy;
	
	static inline
	char* mempcpy( char* p, const plus::string& s )
	{
		return (char*) mempcpy( p, s.data(), s.size() );
	}
	
	
	MB32::MB32( uint32_t mb )
	:
		Value( (const vu_string&) plus::string( (char*) &mb, 4 ).move(),
		       Value_mb32,
		       &mb32_dispatch )
	{
	}
	
	MB32::MB32( const plus::string& s )
	:
		Value( (const vu_string&) s, Value_mb32, &mb32_dispatch )
	{
		if ( s.size() != 4 )
		{
			THROW( "multibyte conversion requires 4 bytes" );
		}
	}
	
	Value MB32::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_empty_list:
				return MB32();
			
			case Value_vector:
				return MB32( v.string() );
			
			case Value_string:
				try
				{
					return MB32( mac_from_utf8( v.string() ) );
				}
				catch ( const chars::unrepresentable_character& )
				{
					THROW( "Unicode character not representable in mb32" );
				}
			
			default:
				THROW( "multibyte conversion not defined for type" );
			
			case Value_mb32:
				return v;
		}
	}
	
	static
	size_t mb32_str_size( const Value& v )
	{
		return utf8_from_mac( v.string() ).size();
	}
	
	static
	char* mb32_str_copy( char* p, const Value& v )
	{
		return mempcpy( p, utf8_from_mac( v.string() ) );
	}
	
	static const stringify mb32_str =
	{
		NULL,
		&mb32_str_size,
		&mb32_str_copy,
	};
	
	static
	size_t mb32_rep_size( const Value& v )
	{
		const char* p = v.string().data();
		
		unsigned length = 0;
		
		length += quoted_length( *p++ );
		length += quoted_length( *p++ );
		length += quoted_length( *p++ );
		length += quoted_length( *p++ );
		
		return length - 3 * 2;  // subtract three pairs of quotes
	}
	
	static
	char* mb32_rep_copy( char* p, const Value& v )
	{
		const char* q = v.string().data();
		
		*p++ = '\'';
		p = copy_quotable_byte( p, *q++ );
		p = copy_quotable_byte( p, *q++ );
		p = copy_quotable_byte( p, *q++ );
		p = copy_quotable_byte( p, *q++ );
		*p++ = '\'';
		
		return p;
	}
	
	static const stringify mb32_rep =
	{
		NULL,
		&mb32_rep_size,
		&mb32_rep_copy,
	};
	
	static
	const char* mb32_vec_data( const Value& v )
	{
		return v.string().data();
	}
	
	static
	size_t mb32_vec_size( const Value& v )
	{
		return 4;
	}
	
	static const stringify mb32_vec =
	{
		&mb32_vec_data,
		&mb32_vec_size,
		NULL,
	};
	
	static const stringifiers mb32_stringifiers =
	{
		&mb32_str,
		&mb32_rep,
		&mb32_vec,
	};
	
	const dispatch mb32_dispatch =
	{
		&mb32_stringifiers,
	};
	
	const type_info mb32_vtype =
	{
		"mb32",
		&assign_to< MB32 >,
		&MB32::coerce,
		0,
	};
	
}
