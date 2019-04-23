/*
	packed.cc
	---------
*/

#include "vlib/types/packed.hh"

// math
#include "math/bitwise.hh"

// gear
#include "gear/hexadecimal.hh"

// vlib
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/pointer.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"
#include "vlib/types/vbytes.hh"


namespace vlib
{
	
	Value Packed::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_packed:
				return v;
			
			default:
				THROW( "type can't be packed" );
			
			case Value_empty_list:
				return Packed();
			
			case Value_string:
				return Packed( v.string() );
			
			case Value_byte:
			case Value_mb32:
			case Value_pair:
			case Value_other:
				return Packed( pack( v ) );
		}
	}
	
	static
	size_t packed_str_size( const Value& v )
	{
		return v.string().size() * 2;
	}
	
	static
	char* packed_str_copy( char* p, const Value& v )
	{
		const plus::string& s = v.string();
		
		return gear::hexpcpy_lower( p, s.data(), s.size() );
	}
	
	static const stringify packed_str =
	{
		NULL,
		&packed_str_size,
		&packed_str_copy,
	};
	
	static
	size_t packed_rep_size( const Value& v )
	{
		return packed_str_size( v ) + 3;
	}
	
	static
	char* packed_rep_copy( char* p, const Value& v )
	{
		*p++ = 'x';
		*p++ = '"';
		
		p = packed_str_copy( p, v );
		
		*p++ = '"';
		
		return p;
	}
	
	static const stringify packed_rep =
	{
		NULL,
		&packed_rep_size,
		&packed_rep_copy,
	};
	
	static const stringifiers packed_stringifiers =
	{
		&packed_str,
		&packed_rep,
		&vbytes_cpy,  // bin
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
				return Type( packed_vtype );
			
			case Op_unary_minus:
				return reversed_bytes( v );
			
			case Op_unary_negate:
				return negated_bytes( v );
			
			case Op_begin:
				return Pointer( v );
			
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
	
	static inline
	Value flatten( const Value& v, const Value& type_sample )
	{
		return VBytes( type_sample.type() == V_str ? str ( v )
		                                           : pack( v ),
		               type_sample.type(),
		               type_sample.dispatch_methods() );
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return flatten( Value( a, b ), b.is< String >() ? b : a );
			
			case Op_intersection:
			case Op_exclusion:
			case Op_union:
				if ( b.type() != V_pack )
				{
					THROW( "bitwise logic requires packed operands" );
				}
				
				if ( a.string().size() != b.string().size() )
				{
					THROW( "bitwise logic operands must be of equal length" );
				}
				
				return binary_bitwise_op( op, a, b.string().data() );
			
			case Op_multiply:
				return multiply( (const VBytes&) a, b );
			
			case Op_divide:
				return division( (const VBytes&) a, b );
			
			case Op_contains:
				return vbytes_contains( a, b );
			
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
	
	const dispatch packed_dispatch =
	{
		&packed_stringifiers,
		NULL,
		&vbytes_comparison,
		&ops,
	};
	
	static
	Value packed_member( const Value&         obj,
	                     const plus::string&  member )
	{
		if ( member == "size" )
		{
			return Integer( obj.string().size() );
		}
		
		if ( member == "string" )
		{
			return String( obj.string() );
		}
		
		THROW( "nonexistent packed data member" );
		
		return Value();
	}
	
	const type_info packed_vtype =
	{
		"packed",
		&assign_to< Packed >,
		&Packed::coerce,
		&packed_member,
		Type_pure,
	};
	
}
