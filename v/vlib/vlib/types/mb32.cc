/*
	mb32.cc
	-------
*/

#include "vlib/types/mb32.hh"

// Standard C
#include <string.h>

// iota
#include "iota/endian.hh"

// more-libc
#include "more/string.h"

// chars
#include "charsets/extended_ascii.hh"

// vxo
#include "vxo/lib/quote.hh"

// plus
#include "plus/mac_utf8.hh"

// vlib
#include "vlib/quote.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/type.hh"


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
		Value( Value_mb32, &mb32_dispatch )
	{
		pod_cast< uint32_t >() = mb;
	}
	
	MB32::MB32( const plus::string& s )
	:
		Value( Value_mb32, &mb32_dispatch )
	{
		if ( s.size() != 4 )
		{
			THROW( "multibyte conversion requires 4 bytes" );
		}
		
		const uint32_t data_BE = *(const uint32_t*) s.data();
		
		pod_cast< uint32_t >() = iota::u32_from_big( data_BE );
	}
	
	Value MB32::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_empty_list:
				return MB32();
			
			case Value_packed:
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
	plus::string utf8_from_mb32( uint32_t mb )
	{
		plus::string s( (const char*) &mb, sizeof mb );
		
		return utf8_from_mac( s );
	}
	
	static
	size_t mb32_str_size( const Value& v )
	{
		const MB32& mb32 = (const MB32&) v;
		
		return utf8_from_mb32( mb32.get() ).size();
	}
	
	static
	char* mb32_str_copy( char* p, const Value& v )
	{
		const MB32& mb32 = (const MB32&) v;
		
		return mempcpy( p, utf8_from_mb32( iota::big_u32( mb32.get() ) ) );
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
		const MB32& mb32 = (const MB32&) v;
		
		const char* p = (const char*) &mb32.get();
		
		unsigned length = 0;
		
		length += vxo::quoted_length( *p++ );
		length += vxo::quoted_length( *p++ );
		length += vxo::quoted_length( *p++ );
		length += vxo::quoted_length( *p++ );
		
		return length - 3 * 2;  // subtract three pairs of quotes
	}
	
	static
	char* mb32_rep_copy( char* p, const Value& v )
	{
		const MB32& mb32 = (const MB32&) v;
		
		const uint32_t data_BE = iota::big_u32( mb32.get() );
		
		const char* q = (const char*) &data_BE;
		
		*p++ = '\'';
		p = vxo::copy_quotable_byte( p, *q++ );
		p = vxo::copy_quotable_byte( p, *q++ );
		p = vxo::copy_quotable_byte( p, *q++ );
		p = vxo::copy_quotable_byte( p, *q++ );
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
	const char* mb32_bin_data( const Value& v )
	{
		const MB32& mb32 = (const MB32&) v;
		
		return (const char*) &mb32.get();
	}
	
	static
	size_t mb32_bin_size( const Value& v )
	{
		return 4;
	}
	
	static
	char* mb32_bin_copy( char* p, const Value& v )
	{
		const MB32& mb32 = (const MB32&) v;
		
		const uint32_t data_BE = iota::big_u32( mb32.get() );
		
		const char* q = (const char*) &data_BE;
		
		return (char*) mempcpy( p, q, sizeof data_BE );
	}
	
	static const stringify mb32_bin =
	{
		iota::is_little_endian() ? NULL : &mb32_bin_data,
		&mb32_bin_size,
		iota::is_little_endian() ? &mb32_bin_copy : NULL,
	};
	
	static const stringifiers mb32_stringifiers =
	{
		&mb32_str,
		&mb32_rep,
		&mb32_bin,
	};
	
	static
	bool mb32_verity( const Value& v )
	{
		const MB32& mb32 = (const MB32&) v;
		
		return mb32.get() != 0;
	}
	
	static const veritization mb32_veritization =
	{
		&mb32_verity,
	};
	
	static inline
	cmp_t compare_u32( uint32_t a, uint32_t b )
	{
		return   a < b ? -1
		       : a > b ?  1
		       :          0;
	}
	
	static
	cmp_t mb32_order( const Value& a, const Value& b )
	{
		const MB32& one = (const MB32&) a;
		const MB32& two = (const MB32&) b;
		
		return compare_u32( one.get(), two.get() );
	}
	
	const comparison mb32_comparison =
	{
		&mb32_order,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( mb32_vtype );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
	};
	
	const dispatch mb32_dispatch =
	{
		&mb32_stringifiers,
		&mb32_veritization,
		&mb32_comparison,
		&ops,
	};
	
	const type_info mb32_vtype =
	{
		"mb32",
		&assign_to< MB32 >,
		&MB32::coerce,
		0,
		0,
		Type_pure,
	};
	
}
