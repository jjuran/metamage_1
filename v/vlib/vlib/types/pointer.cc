/*
	pointer.cc
	----------
*/

#include "vlib/types/pointer.hh"

// POSIX
#include <sys/types.h>

// gear
#include "gear/find.hh"

// debug
#include "debug/assert.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/byterange.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"
#include "vlib/types/vbytes.hh"


namespace vlib
{
	
	extern const type_info iterator_vtype;
	
	static
	const char* pointer_str_data( const Value& v )
	{
		return "<pointer>";
	}
	
	static const stringify pointer_str =
	{
		&pointer_str_data,
		NULL,
		NULL,
	};
	
	static const stringifiers pointer_stringifiers =
	{
		&pointer_str,
	};
	
	static
	bool pointer_truth( const Value& v )
	{
		// Check if the pointer is at the end.
		
		Expr* expr = v.expr();
		
		const plus::string&    s = expr->left.string();
		const bignum::integer& i = expr->right.number();
		
		return i < s.size();
	}
	
	static const veritization pointer_veritization =
	{
		&pointer_truth,
	};
	
	static
	cmp_t pointer_order( const Value& a, const Value& b )
	{
		Expr* ax = a.expr();
		Expr* bx = b.expr();
		
		const Value& a_bytes = ax->left;
		const Value& b_bytes = bx->left;
		
		if ( &a_bytes != &b_bytes  &&  a_bytes.string() != b_bytes.string() )
		{
			THROW( "comparison of pointers to different containers" );
		}
		
		return compare( ax->right.number(), bx->right.number() );
	}
	
	static const comparison pointer_comparison =
	{
		&pointer_order,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( pointer_vtype );
			
			case Op_unary_deref:
			{
				Expr* expr = v.expr();
				
				const plus::string& s = expr->left.string();
				
				ASSERT( integer_cast< size_t >( expr->right ) <= s.size() );
				
				const size_t i = expr->right.number().clipped();
				
				if ( i >= s.size() )
				{
					return empty_list;
				}
				
				return Byte( s[ i ] );
			}
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value substring( const Value& v, size_t i, size_t n = plus::string::npos )
	{
		const plus::string substring = v.string().substr( i, n );
		
		return VBytes( substring, v.type(), v.dispatch_methods() );
	}
	
	static
	Value substring( const Value& p, const Value& q )
	{
		if ( Pointer::test( p ) )
		{
			const plus::string& a = p.expr()->left.string();
			const plus::string& b = q.expr()->left.string();
			
			if ( &a == &b  ||  a == b )
			{
				const size_t i = integer_cast< size_t >( p.expr()->right );
				const size_t j = integer_cast< size_t >( q.expr()->right );
				
				if ( i > j )
				{
					THROW( "negative pointer subtraction" );
				}
				
				return substring( p.expr()->left, i, j - i );
			}
			
			THROW( "subtraction of pointers from different containers" );
		}
		
		return Value();
	}
	
	static
	const Value& advance( Value& v, ssize_t delta )
	{
		Expr* expr = v.unshare().expr();
		
		const plus::string& s = expr->left.string();
		bignum::integer& iter = expr->right.number();
		
		const size_t i = integer_cast< size_t >( iter );
		const size_t j = i + delta;
		
		if ( delta < 0  &&  delta < -(ssize_t) i )
		{
			THROW( "pointer arithmetic underrun" );
		}
		
		if ( j > s.size() )
		{
			THROW( "pointer arithmetic overrun" );
		}
		
		iter = j;  // Don't bother swapping, since a size_t is never a bigint
		
		return v;
	}
	
	static
	const Value& advance( Value& v, const Value& delta, bool reverse )
	{
		if ( delta.type() != V_int )
		{
			THROW( "pointer arithmetic requires numeric operand" );
		}
		
		bignum::integer n = delta.number();
		
		if ( reverse )
		{
			n.invert();
		}
		
		return advance( v, integer_cast< ssize_t >( n ) );
	}
	
	static
	const Value& scan( Value& v, const plus::string& pattern )
	{
		Expr* expr = v.unshare().expr();
		
		const plus::string& s = expr->left.string();
		bignum::integer& iter = expr->right.number();
		
		const size_t i = integer_cast< size_t >( iter );
		
		const size_t n = pattern.size();
		const size_t j = i + n;  // can't overflow due to limited size_type
		
		if ( j <= s.size()  &&  s.substr( i, n ) == pattern )
		{
			iter += n;
			
			return v;
		}
		
		return empty_list;  // No match
	}
	
	static
	const Value& scan( Value& v, const iota::byte_range& pattern )
	{
		Expr* expr = v.unshare().expr();
		
		const plus::string& s = expr->left.string();
		bignum::integer& iter = expr->right.number();
		
		const size_t i = integer_cast< size_t >( iter );
		
		if ( i < s.size()  &&  contains( pattern, s[ i ] ) )
		{
			++iter;
			
			return v;
		}
		
		return empty_list;  // No match
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		if ( op == Op_subtract )
		{
			return substring( b, a );
		}
		
		Value v = a;
		
		if ( op == Op_add )
		{
			if ( b.type() == v.expr()->left.type() )
			{
				// E.g. `p + "pattern"`
				
				return scan( v, b.string() );
			}
			
			if ( const ByteRange* byterange = b.is< ByteRange >() )
			{
				return scan( v, byterange->get() );
			}
		}
		else if ( op != Op_subtract )
		{
			return Value();  // unimplemented
		}
		
		return advance( v, b, op == Op_subtract );
	}
	
	static
	Value advance_op_handler( op_type op, const Target& target )
	{
		Value& v = *target.addr;
		
		switch ( op )
		{
			case Op_preinc:
				return advance( v, 1 );
			
			case Op_predec:
				return advance( v, -1 );
			
			case Op_postinc:
				{
					Value result = v;
					
					advance( v, 1 );
					
					return result;
				}
			
			case Op_postdec:
				{
					Value result = v;
					
					advance( v, -1 );
					
					return result;
				}
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value mutating_op_handler( op_type        op,
	                           const Target&  target,
	                           const Value&   x,
	                           const Value&   b )
	{
		Value& v = *target.addr;
		
		if ( op == Op_increase_by )
		{
			if ( b.type() == v.expr()->left.type() )
			{
				// E.g. `p += "pattern"`
				
				return scan( v, b.string() );
			}
			
			if ( const ByteRange* byterange = b.is< ByteRange >() )
			{
				return scan( v, byterange->get() );
			}
		}
		else if ( op != Op_decrease_by )
		{
			return Value();  // unimplemented
		}
		
		return advance( v, b, op == Op_decrease_by );
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
		&advance_op_handler,
		&mutating_op_handler,
	};
	
	const dispatch pointer_dispatch =
	{
		&pointer_stringifiers,
		&pointer_veritization,
		&pointer_comparison,
		&ops,
	};
	
	Pointer::Pointer( const Value& container )
	:
		Value( container, Op_subscript, Integer(), &pointer_dispatch )
	{
	}
	
	Pointer::Pointer( const Value& container, Max )
	:
		Value( container,
		       Op_subscript,
		       Integer( container.string().size() ),
		       &pointer_dispatch )
	{
	}
	
	static
	Value v_find( const Value& v )
	{
		const Value& pointer = first( v );
		const Value& target  = rest ( v );
		
		const Value& container = pointer.expr()->left;
		const Value& index     = pointer.expr()->right;
		
		const plus::string& s = container.string();
		
		const size_t i = integer_cast< size_t >( index );
		
		size_t loc = plus::string::npos;
		
		if ( const Byte* byte = target.is< Byte >() )
		{
			const char c = byte->get();
			
			loc = s.find( c, i );
		}
		else if ( const ByteRange* byterange = target.is< ByteRange >() )
		{
			const iota::byte_range br = byterange->get();
			
			const char* begin = s.data();
			const char* match = gear::find_first_match( begin, s.size(), br );
			
			loc = match ? match - begin : plus::string::npos;
		}
		else
		{
			const plus::string& sub = target.string();
			
			loc = s.find( sub );
		}
		
		if ( loc == plus::string::npos )
		{
			return empty_list;
		}
		
		Pointer result( container );
		
		advance( result, loc );
		
		return result;
	}
	
	static const Type pointer = pointer_vtype;
	static const Type byte    = byte_vtype;
	static const Type brange  = byterange_vtype;
	static const Type packed  = packed_vtype;
	static const Type string  = string_vtype;
	
	static const Value vbytes ( string, Op_union, packed );
	static const Value a_byte ( byte,   Op_union, brange );
	static const Value finding( a_byte, Op_union, vbytes );
	
	static const Value find( pointer, finding );
	
	static const proc_info proc_find = { "find", &v_find, &find };
	
	static
	Value pointer_member( const Value&         v,
	                      const plus::string&  member )
	{
		if ( member == "min" )
		{
			return Pointer( v.expr()->left );
		}
		
		if ( member == "max" )
		{
			return Pointer( v.expr()->left, Pointer::Max() );
		}
		
		const Value& container = v.expr()->left;
		
		const size_t i = integer_cast< size_t >( v.expr()->right );
		
		if ( member == "past" )
		{
			return substring( container, 0, i );
		}
		
		if ( member == "rest" )
		{
			return substring( container, i );
		}
		
		if ( member == "find" )
		{
			return bind_args( Proc( proc_find ), v );
		}
		
		THROW( "nonexistent pointer member" );
		
		return Value();
	}
	
	const type_info pointer_vtype =
	{
		"pointer",
		&assign_to< Pointer >,
		NULL,
		&pointer_member,
		Type_pure,
	};
	
}
