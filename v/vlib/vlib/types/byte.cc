/*
	byte.cc
	-------
*/

#include "vlib/types/byte.hh"

// plus
#include "plus/integer.hh"

// vlib
#include "vlib/quote.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"


namespace vlib
{
	
	Value Byte::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_byte:
				return v;
			
			default:
				THROW( "byte conversion not defined for type" );
			
			case Value_empty_list:
				return Byte();
			
			case Value_number:
				return Byte( v.number().clipped() * v.number().sign() );
		}
	}
	
	static
	size_t byte_str_size( const Value& v )
	{
		return 1;
	}
	
	static
	char* byte_str_copy( char* p, const Value& v )
	{
		*p++ = v.number().clipped();
		
		return p;
	}
	
	static const stringify byte_str =
	{
		NULL,
		&byte_str_size,
		&byte_str_copy,
	};
	
	static
	size_t byte_rep_size( const Value& v )
	{
		return quoted_length( v.number().clipped() );
	}
	
	static
	char* byte_rep_copy( char* p, const Value& v )
	{
		return quote_byte( p, v.number().clipped() );
	}
	
	static const stringify byte_rep =
	{
		NULL,
		&byte_rep_size,
		&byte_rep_copy,
	};
	
	static const stringifiers byte_stringifiers =
	{
		&byte_str,
		&byte_rep,
		&byte_str,  // reuse str for bin
	};
	
	static
	bool byte_verity( const Value& v )
	{
		return ! v.number().is_zero();
	}
	
	static const veritization byte_veritization =
	{
		&byte_verity,
	};
	
	static
	cmp_t byte_order( const Value& a, const Value& b )
	{
		return compare( a.number(), b.number() );
	}
	
	static const comparison byte_comparison =
	{
		&byte_order,
	};
	
	const dispatch byte_dispatch =
	{
		&byte_stringifiers,
		&byte_veritization,
		&byte_comparison,
	};
	
	const type_info byte_vtype =
	{
		"byte",
		&assign_to< Byte >,
		&Byte::coerce,
		0,
	};
	
}
