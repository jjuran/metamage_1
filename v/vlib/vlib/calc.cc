/*
	calc.cc
	-------
*/

#include "vlib/calc.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/types.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
	using math::integer::cmp_t;
	
	
	static
	const plus::integer& nonzero( const plus::integer& x )
	{
		if ( x.is_zero() )
		{
			DOMAIN_ERROR( "division by zero" );
		}
		
		return x;
	}
	static
	bool isa( const Value& v, const Value& t )
	{
		if ( t.type() != Value_base_type )
		{
			// TODO:  Support type lists.
			
			TYPE_ERROR( "`isa` right operand must be a type" );
		}
		
		const type_info& typeinfo = t.typeinfo();
		
		return typeinfo.assign( v ).type();
	}
	
	static
	bool equal_atoms( const Value& a, const Value& b )
	{
		if ( get_type( a ) != get_type( b ) )
		{
			if ( is_empty( a )  ||  is_empty( b ) )
			{
				return false;
			}
			
			TYPE_ERROR( "mismatched types in equality relation" );
		}
		
		switch ( get_type( a ) )
		{
			case Value_empty_list:
				return true;
			
			case Value_boolean:
			case Value_number:
				return get_int( a ) == get_int( b );
			
			case Value_string:
				return get_str( a ) == get_str( b );
			
			case Value_function:
				return &get_proc( a ) == &get_proc( b );
			
			case Value_base_type:
				return &a.typeinfo() == &b.typeinfo();
			
			case Value_pair:
				{
					Expr& ax = *get_expr( a );
					Expr& bx = *get_expr( b );
					
					return ax.op == bx.op                     &&
					       equal_atoms( ax.left,  bx.left  )  &&
					       equal_atoms( ax.right, bx.right );
				}
			
			default:
				INTERNAL_ERROR( "unsupported type in equal_atoms()" );
				break;
		}
		
		return false;
	}
	
	static
	bool equal( const Value& one, const Value& two )
	{
		const Value* a = &one;
		const Value* b = &two;
		
	next:
		
		if ( ! equal_atoms( first( *a ), first( *b ) ) )
		{
			return false;
		}
		
		if ( get_type( *a ) != Value_pair  &&  get_type( *b ) != Value_pair )
		{
			return true;
		}
		
		a = &rest( *a );
		b = &rest( *b );
		
		goto next;
	}
	
	static
	cmp_t compare( const Value& a, const Value& b )
	{
		if ( get_type( a ) != get_type( b ) )
		{
			TYPE_ERROR( "mismatched types in compare()" );
		}
		
		switch ( get_type( a ) )
		{
			case Value_number:
				return compare( get_int( a ), get_int( b ) );
			
			case Value_string:
				return compare( get_str( a ), get_str( b ) );
			
			default:
				TYPE_ERROR( "unsupported type in compare()" );
				break;
		}
		
		return 0;
	}
	
	static
	Value calc_unary( op_type op, const Value& v )
	{
		if ( op == Op_const  ||  op == Op_var )
		{
			SYNTAX_ERROR( "const/var operand not a symbol" );
		}
		
		if ( op == Op_unary_count )
		{
			return count( v );
		}
		
		switch ( get_type( v ) )
		{
			case Value_empty_list:
				return 0;
			
			case Value_boolean:
			case Value_number:
				switch ( op )
				{
					case Op_unary_plus:   return  get_int( v );
					case Op_unary_minus:  return -get_int( v );
					
					default:  break;
				}
				
				break;
			
			case Value_string:
				SYNTAX_ERROR( "unary operator not defined for string values" );
				break;
			
			case Value_pair:
				SYNTAX_ERROR( "unary operator not defined for lists" );
				break;
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in calc_unary()" );
		
		return Value();
	}
	
	static
	Value calc_member( const Value& left, const Value& right )
	{
		if ( get_type( right ) != Value_string )
		{
			SYNTAX_ERROR( "non-string member name" );
		}
		
		Value vtype = proc_typeof.addr( left );
		
		if ( vtype.type() == Value_base_type )
		{
			const type_info& typeinfo = vtype.typeinfo();
			
			if ( member_proc member = typeinfo.member )
			{
				return member( left, right.string() );
			}
		}
		
		SYNTAX_ERROR( "unsupported type for member access" );
		
		return Value();
	}
	
	static
	Value calc( const plus::integer&  left,
	            op_type               op,
	            const plus::integer&  right )
	{
		switch ( op )
		{
			case Op_add:       return left + right;
			case Op_subtract:  return left - right;
			case Op_multiply:  return left * right;
			case Op_divide:    return left / nonzero( right );
			case Op_remain:    return left % nonzero( right );
			case Op_modulo:    return modulo( left, nonzero( right ) );
			
			case Op_empower:   return raise_to_power( left, right );
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in calc()" );
		
		return Value();
	}
	
	static
	plus::string repeat_string( const plus::string& s, const plus::integer& n )
	{
		typedef plus::string::size_type size_t;
		
		if ( n.is_negative() )
		{
			DOMAIN_ERROR( "negative string multiplier" );
		}
		
		if ( n > size_t( -1 ) )
		{
			DOMAIN_ERROR( "excessively large string multiplier" );
		}
		
		return repeat( s, n.clipped() );
	}
	
	static
	Value repeat_list( const Value& list, const Value& factor )
	{
		if ( get_type( factor ) != Value_number )
		{
			TYPE_ERROR( "non-numeric list repetition factor" );
		}
		
		if ( get_int( factor ).is_negative() )
		{
			DOMAIN_ERROR( "negative list repetition factor" );
		}
		
		if ( get_int( factor ).is_zero()  ||  is_empty( list ) )
		{
			return Value_empty_list;
		}
		
		if ( get_int( factor ) > 0xFFFFFFFFu )
		{
			DOMAIN_ERROR( "excessively large list multiplier" );
		}
		
		unsigned long n = get_int( factor ).clipped();
		
		if ( n == 1 )
		{
			return list;
		}
		
		Value result = list;
		
		while ( --n > 0 )
		{
			result = make_pair( list, result );
		}
		
		return result;
	}
	
	static
	Value call_function( const Value& f, const Value& arguments )
	{
		if ( get_type( f ) == Value_function )
		{
			return get_proc( f ).addr( arguments );
		}
		
		if ( f.type() == Value_base_type )
		{
			const type_info& typeinfo = f.typeinfo();
			
			if ( coerce_proc coerce = typeinfo.coerce )
			{
				return coerce( arguments );
			}
			
			const Value coerced = typeinfo.assign( arguments );
			
			if ( coerced.type() )
			{
				return coerced;
			}
			
			TYPE_ERROR( "invalid type conversion arguments" );
		}
		
		if ( Expr* expr = get_expr( f ) )
		{
			const Value& method = expr->left;
			const Value& object = expr->right;
			
			return call_function( method, calc( object, Op_list, arguments ) );
		}
		
		TYPE_ERROR( "attempted call of non-function" );
		
		return Value();  // not reached
	}
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		if ( get_type( left ) == Value_dummy_operand )
		{
			return calc_unary( op, right );
		}
		
		switch ( op )
		{
			case Op_member:
				return calc_member( left, right );
			
			case Op_function:
			case Op_named_unary:
				return call_function( left, right );
			
			case Op_isa:      return isa( left, right );
			case Op_equal:    return equal( left, right );
			case Op_unequal:  return ! equal( left, right );
			
			case Op_lt:   return compare( left, right ) <  0;
			case Op_lte:  return compare( left, right ) <= 0;
			case Op_gt:   return compare( left, right ) >  0;
			case Op_gte:  return compare( left, right ) >= 0;
			
			default:  break;
		}
		
		if ( op == Op_list )
		{
			if ( is_empty( left ) )
			{
				return right;
			}
			
			if ( is_empty( right ) )
			{
				return left;
			}
			
			return make_pair( left, right );
		}
		
		if ( op == Op_repeat )
		{
			return repeat_list( left, right );
		}
		
		if ( op == Op_bind_args  &&  is_functional( left ) )
		{
			return bind_args( left, right );
		}
		
		if ( get_type( left ) == get_type( right ) )
		{
			switch ( get_type( left ) )
			{
				case Value_empty_list:
					SYNTAX_ERROR( "operator not defined for empty list" );
				
				case Value_boolean:
					SYNTAX_ERROR( "operator not defined for boolean values" );
				
				case Value_number:
					return calc( get_int( left ), op, get_int( right ) );
				
				case Value_string:
					SYNTAX_ERROR( "operator not defined for string values" );
				
				case Value_function:
					SYNTAX_ERROR( "operator not defined for functions" );
				
				case Value_base_type:
					SYNTAX_ERROR( "operator not defined for types" );
				
				case Value_pair:
					SYNTAX_ERROR( "operator not defined for lists" );
				
				default:
					break;
			}
		}
		
		if ( op == Op_multiply  &&  get_type( left ) == Value_string )
		{
			if ( get_type( right ) == V_bool  ||  get_type( right ) == V_int )
			{
				return repeat_string( get_str( left ), get_int( right ) );
			}
		}
		
		SYNTAX_ERROR( "operator not defined on mixed types" );
		
		return Value();
	}
	
}
