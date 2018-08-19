/*
	byterange.cc
	------------
*/

#include "vlib/types/byterange.hh"

// vlib
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/byteclass.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	plus::string byterange_rep_make( const Value& v )
	{
		iota::byte_range br = v.as< ByteRange >().get();
		
		if ( empty( br ) )
		{
			return "('\0' -> '\0')";
		}
		
		return rep( Value( Byte( front( br ) ), Op_gamut,
		                   Byte( back ( br ) ) ) );
	}
	
	static const stringify byterange_rep =
	{
		NULL,
		NULL,
		NULL,
		&byterange_rep_make,
	};
	
	static
	size_t byterange_bin_size( const Value& v )
	{
		return size( v.as< ByteRange >().get() );
	}
	
	static
	char* byterange_bin_copy( char* p, const Value& v )
	{
		iota::byte_range br = v.as< ByteRange >().get();
		
		if ( empty( br ) )
		{
			return p;
		}
		
		uint8_t lower = front( br );
		uint8_t upper = back ( br );
		
		uint8_t i = lower;
		
		do
		{
			*p++ = i;
		}
		while ( i++ < upper );
		
		return p;
	}
	
	static const stringify byterange_bin =
	{
		NULL,
		&byterange_bin_size,
		&byterange_bin_copy,
	};
	
	static const stringifiers byterange_stringifiers =
	{
		NULL,            // str: undefined
		&byterange_rep,  // rep: range syntax (always gamut, never delta)
		&byterange_bin,  // bin: packed array of bytes in sequence
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( byterange_vtype );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static inline
	bool byterange_contains( const Value& a, const Value& b )
	{
		iota::byte_range byterange = static_cast< const ByteRange& >( a ).get();
		
		if ( const Byte* byte = b.is< Byte >() )
		{
			return contains( byterange, byte->get() );
		}
		
		THROW( "unsupported pattern type for `in` with byterange" );
		return false;
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_union:
				return a.to< ByteClass >() | b.to< ByteClass >();
			
			case Op_contains:
				return Boolean( byterange_contains( a, b ) );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	const dispatch byterange_dispatch =
	{
		&byterange_stringifiers,
		NULL,
		NULL,
		&ops,
	};
	
	ByteRange::ByteRange( const Value& left, op_type op, const Value& right )
	:
		Value( Value_other, &byterange_dispatch )
	{
		uint8_t a = left .to< Byte >().get();
		uint8_t z = right.to< Byte >().get() - (op == Op_delta);
		
		pod_cast< iota::byte_range >() = iota::make_byte_range( a, z );
	}
	
	const type_info byterange_vtype =
	{
		"byterange",
		&assign_to< ByteRange >,
		NULL,
		NULL,
		Type_pure,
	};
	
}
