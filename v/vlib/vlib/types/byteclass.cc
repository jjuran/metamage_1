/*
	byteclass.cc
	------------
*/

#include "vlib/types/byteclass.hh"

// math
#include "math/bitwise.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/byterange.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/type.hh"
#include "vlib/types/vbytes.hh"


namespace vlib
{
	
	static
	const vu_string& null_byteclass_bits()
	{
		static plus::string null( 32, '\0' );
		
		return (const vu_string&) null;
	}
	
	Value ByteClass::coerce( const Value& v )
	{
		if ( v.is< ByteClass >() )
		{
			return v;
		}
		
		if ( v.is< ByteRange >() )
		{
			iota::byte_range br = v.as< ByteRange >().get();
			
			uint16_t a = begin( br );
			uint16_t b = end  ( br );
			
			ByteClass result;
			
			for ( uint16_t i = a;  i < b;  ++i )
			{
				result.set( i );
			}
			
			return result;
		}
		
		switch ( v.type() )
		{
			case Value_empty_list:
				return ByteClass();
			
			case Value_byte:
				return ByteClass( v.as< Byte >().get() );
			
			default:
				THROW( "type can't be coerced to byteclass" );
		}
		
		return Value();  // not reached
	}
	
	static
	const char* byteclass_str_data( const Value& v )
	{
		return "<byteclass>";
	}
	
	static const stringify byteclass_str =
	{
		&byteclass_str_data,
		NULL,
		NULL,
	};
	
	static const stringifiers byteclass_stringifiers =
	{
		&byteclass_str,
		NULL,  // rep
		NULL,  // bin
	};
	
	static
	Value negated_bytes( Value v )
	{
		using namespace math::bitwise;
		
		const plus::string& s = v.unshare().string();
		
		bitwise_not( (char*) s.data(), s.size() );
		
		return v;
	}
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( byteclass_vtype );
			
			case Op_unary_negate:
				return negated_bytes( v );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_bitwise_op( op_type op, Value a, const char* b )
	{
		using namespace math::bitwise;
		
		const plus::string& s = a.unshare().string();
		
		char* data = (char*) s.data();
		
		const plus::string::size_type n = s.size();
		
		switch ( op )
		{
			default:  // won't happen
			
			case Op_intersection:  bitwise_and( data, b, n );  break;
			case Op_exclusion:     bitwise_xor( data, b, n );  break;
			case Op_union:         bitwise_or ( data, b, n );  break;
		}
		
		return a;
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_intersection:
			case Op_exclusion:
			case Op_union:
				return binary_bitwise_op( op, a, b.to< ByteClass >().string().data() );
			
			case Op_contains:
				return Boolean( a.as< ByteClass >().get( b.to< Byte >().get() ) );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
		NULL,
		&vbytes_mutating_op_handler,
	};
	
	const dispatch byteclass_dispatch =
	{
		&byteclass_stringifiers,
		NULL,
		&vbytes_comparison,
		&ops,
	};
	
	ByteClass::ByteClass()
	:
		Value( null_byteclass_bits(), Value_other, &byteclass_dispatch )
	{
		set_evaluated();
	}
	
	ByteClass::ByteClass( unsigned char c )
	:
		Value( null_byteclass_bits(), Value_other, &byteclass_dispatch )
	{
		set_evaluated();
		
		set( c );
	}
	
	ByteClass::ByteClass( unsigned char a, unsigned char b )
	:
		Value( null_byteclass_bits(), Value_other, &byteclass_dispatch )
	{
		set_evaluated();
		
		set( a );
		set( b );
	}
	
	bool ByteClass::get( unsigned char i ) const
	{
		const char* data = string().data();
		
		return data[ i >> 3 ] & 1 << (i & 0x7);
	}
	
	void ByteClass::set( unsigned char i )
	{
		char* data = const_cast< char* >( unshare().string().data() );
		
		data[ i >> 3 ] |= 1 << (i & 0x7);
	}
	
	ByteClass operator|( const ByteClass& a, const ByteClass& b )
	{
		Value result = binary_bitwise_op( Op_union, a, b.string().data() );
		
		return static_cast< const ByteClass& >( result );
	}
	
	static
	Value byteclass_member( const Value&         obj,
	                        const plus::string&  member )
	{
		if ( member == "bitmap" )
		{
			return Packed( obj.string() );
		}
		
		return Value();
	}
	
	const type_info byteclass_vtype =
	{
		"byteclass",
		&assign_to< ByteClass >,
		&ByteClass::coerce,
		&byteclass_member,
		Type_pure,
	};
	
}
