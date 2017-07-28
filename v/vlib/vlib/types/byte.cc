/*
	byte.cc
	-------
*/

#include "vlib/types/byte.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/quote.hh"
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/endec/byte.hh"
#include "vlib/types/field.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


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
	
	unsigned char Byte::get() const
	{
		return pod_cast< unsigned char >();
	}
	
	static
	size_t byte_str_size( const Value& v )
	{
		return 1;
	}
	
	static
	char* byte_str_copy( char* p, const Value& v )
	{
		*p++ = v.to< Byte >().get();
		
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
		return quoted_length( v.to< Byte >().get() );
	}
	
	static
	char* byte_rep_copy( char* p, const Value& v )
	{
		return quote_byte( p, v.to< Byte >().get() );
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
		return v.to< Byte >().get() != '\0';
	}
	
	static const veritization byte_veritization =
	{
		&byte_verity,
	};
	
	static
	cmp_t compare( unsigned char a, unsigned char b )
	{
		return a < b ? -1 :
		       b < a ?  1 :
		                0;
	}
	
	static
	cmp_t byte_order( const Value& a, const Value& b )
	{
		return compare( a.to< Byte >().get(), b.to< Byte >().get() );
	}
	
	static const comparison byte_comparison =
	{
		&byte_order,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( byte_vtype );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		if ( op == Op_function  ||  op == Op_named_unary )
		{
			if ( b.is< String >() )
			{
				return String( str( Value( a, b ) ) );
			}
			
			if ( b.is< Packed >() )
			{
				return Packed( pack( Value( a, b ) ) );
			}
		}
		
		if ( op == Op_delta )
		{
			THROW( "BYTE -> BYTE unimplemented (use BYTE .. BYTE instead)" );
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	const dispatch byte_dispatch =
	{
		&byte_stringifiers,
		&byte_veritization,
		&byte_comparison,
		&ops,
	};
	
	const type_info byte_vtype =
	{
		"byte",
		&assign_to< Byte >,
		&Byte::coerce,
		0,
		Type_pure,
	};
	
	Value byte_type()
	{
		return Field( 1, byte_vtype, proc_encode_byte, proc_decode_byte );
	}
	
}
