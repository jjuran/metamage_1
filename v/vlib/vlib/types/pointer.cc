/*
	pointer.cc
	----------
*/

#include "vlib/types/pointer.hh"

// POSIX
#include <sys/types.h>

// debug
#include "debug/assert.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	extern const type_info iterator_vtype;
	
	static
	const char* iterator_str_data( const Value& v )
	{
		return "<pointer>";
	}
	
	static const stringify iterator_str =
	{
		&iterator_str_data,
		NULL,
		NULL,
	};
	
	static const stringifiers iterator_stringifiers =
	{
		&iterator_str,
	};
	
	static
	bool iterator_truth( const Value& v )
	{
		// Check if the iterator is empty.
		
		Expr* expr = v.expr();
		
		const plus::string&    s = expr->left.string();
		const bignum::integer& i = expr->right.number();
		
		return i < s.size();
	}
	
	static const veritization iterator_veritization =
	{
		&iterator_truth,
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
				
				plus::string substring;
				
				if ( i <= j  && i < a.size() )
				{
					substring = a.substr( i, j - i );
				}
				
				const bool str = q.expr()->left.type() == V_str;
				
				return str ? Value( String( substring ) )
				           : Value( Packed( substring ) );
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
				// E.g. `p += "pattern"`
				
				return scan( v, b.string() );
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
		&iterator_stringifiers,
		&iterator_veritization,
		NULL,
		&ops,
	};
	
	Pointer::Pointer( const Value& container )
	:
		Value( container, Op_subscript, Integer(), &pointer_dispatch )
	{
	}
	
	const type_info pointer_vtype =
	{
		"pointer",
		&assign_to< Pointer >,
		NULL,
		NULL,
	};
	
}
