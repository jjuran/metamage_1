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
#include "vlib/symbol_table.hh"
#include "vlib/types.hh"
#include "vlib/type_info.hh"
#include "vlib/types.hh"


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
		if ( a.type() != b.type() )
		{
			return false;
		}
		
		switch ( a.type() )
		{
			case Value_nothing:
			case Value_dummy_operand:
			case Value_empty_list:
				return true;
			
			case Value_boolean:
			case Value_byte:
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
	bool single_type_mismatch( const Value& one, const Value& two )
	{
		if ( ! is_single( one )  ||  ! is_single( two ) )
		{
			return false;
		}
		
		if ( one.type() != two.type() )
		{
			return true;
		}
		
		if ( one.type() == Value_pair )
		{
			Expr& ax = *one.expr();
			Expr& bx = *two.expr();
			
			return ax.op != bx.op;
		}
		
		return false;
	}
	
	static
	bool equal( const Value& one, const Value& two )
	{
		if ( single_type_mismatch( one, two ) )
		{
			TYPE_ERROR( "mismatched types in equality relation" );
		}
		
		const Value* a = &one;
		const Value* b = &two;
		
	next:
		
		if ( ! equal_atoms( first( *a ), first( *b ) ) )
		{
			return false;
		}
		
		if ( a->type() != Value_pair  &&  b->type() != Value_pair )
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
		if ( a.type() != b.type() )
		{
			TYPE_ERROR( "mismatched types in compare()" );
		}
		
		switch ( a.type() )
		{
			case Value_byte:
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
	Value deref_string( const plus::string& s )
	{
		typedef plus::string::size_type size_t;
		
		const size_t size = s.size();
		
		if ( size == 0 )
		{
			return Value_empty_list;
		}
		
		const char* begin = s.data();
		const char* it    = begin + size;
		
		Value result = Value::byte( *--it );
		
		while ( it > begin )
		{
			result = Value( Value::byte( *--it ), result );
		}
		
		return result;
	}
	
	static
	Value calc_unary( op_type op, const Value& v )
	{
		if ( op == Op_const  ||  op == Op_var )
		{
			SYNTAX_ERROR( "const/var operand not a symbol" );
		}
		
		if ( op == Op_array  ||  op == Op_block )
		{
			return Value( op, v );
		}
		
		if ( op == Op_unary_count )
		{
			return count( v );
		}
		
		if ( Expr* expr = v.expr() )
		{
			switch ( expr->op )
			{
				case Op_list:
					SYNTAX_ERROR( "unary operator not defined for lists" );
				
				case Op_array:
					if ( op == Op_unary_deref )
					{
						return expr->right;
					}
					
					SYNTAX_ERROR( "unary operator not defined for arrays" );
				
				case Op_block:
					SYNTAX_ERROR( "unary operator not defined for blocks" );
				
				default:
					INTERNAL_ERROR( "unary operator not defined for pairs" );
			}
		}
		
		switch ( v.type() )
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
				
				SYNTAX_ERROR( "unary operator not defined for integers" );
				break;
			
			case Value_string:
				if ( op == Op_unary_deref )
				{
					return deref_string( v.string() );
				}
				SYNTAX_ERROR( "unary operator not defined for string values" );
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
		if ( right.type() != Value_string )
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
		if ( factor.type() != Value_number )
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
	
	static inline
	const type_info& get_typeinfo( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return expr->left.typeinfo();
		}
		
		return v.typeinfo();
	}
	
	static inline
	const Value* get_default_arg( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return &expr->right;
		}
		
		return NULL;
	}
	
	static
	Value apply_prototype( const Value& prototype, const Value& arguments )
	{
		if ( is_empty( prototype ) )
		{
			if ( is_empty( arguments ) )
			{
				return arguments;
			}
			
			TYPE_ERROR( "too many arguments" );
		}
		
		const Value& type = first( prototype );
		
		const type_info& typeinfo = get_typeinfo( type );
		
		if ( &typeinfo == &etc_vtype )
		{
			return arguments;
		}
		
		const Value& arg = first( arguments );
		
		const Value* default_arg = get_default_arg( type );
		
		const bool defaulted = is_empty( arg )  &&  default_arg != NULL;
		
		const Value r = typeinfo.assign( defaulted ? *default_arg : arg );
		
		if ( r.type() == Value_nothing )
		{
			TYPE_ERROR( "arguments don't match function prototype" );
		}
		
		const Value remaining = apply_prototype( rest( prototype ),
		                                         rest( arguments ) );
		
		return is_empty( remaining ) ? r : Value( r, remaining );
	}
	
	static
	Value call_function( const Value& f, const Value& arguments )
	{
		if ( f.type() == Value_function )
		{
			const proc_info& proc = f.proc();
			
			if ( const Value* prototype = proc.prototype )
			{
				const Value args = apply_prototype( *prototype, arguments );
				
				return proc.addr( args );
			}
			
			return proc.addr( arguments );
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
			if ( expr->op == Op_block )
			{
				const Value& invoke = expr->left;
				const Value& block  = expr->right;
				
				const symbol_id underscore = locate_symbol( "_" );
				
				const Value previous = underscore->get();
				
				underscore->assign( arguments );
				
				const Value result = invoke.proc().addr( block );
				
				underscore->assign( previous );
				
				return result;
			}
			
			const Value& method = expr->left;
			const Value& object = expr->right;
			
			return call_function( method, calc( object, Op_list, arguments ) );
		}
		
		TYPE_ERROR( "attempted call of non-function" );
		
		return Value();  // not reached
	}
	
	static
	Value string_subscript( const plus::string& s, const Value& i )
	{
		if ( i.type() != Value_number )
		{
			TYPE_ERROR( "non-integer string subscript" );
		}
		
		const plus::integer& index = i.number();
		
		if ( index.is_negative() )
		{
			DOMAIN_ERROR( "negative string subscript" );
		}
		
		if ( index >= s.size() )
		{
			DOMAIN_ERROR( "subscript exceeds string bounds" );
		}
		
		return Value::byte( s[ index.clipped() ] );
	}
	
	static
	Value get_nth( const Value& list, unsigned i )
	{
		const Value* next = &list;
		
		while ( i-- > 0  &&  ! is_empty( *next ) )
		{
			next = &rest( *next );
		}
		
		return first( *next );
	}
	
	static
	Value array_subscript( const Expr& expr, const Value& index )
	{
		if ( index.type() != Value_number )
		{
			TYPE_ERROR( "non-integer array subscript" );
		}
		
		if ( index.number().is_negative() )
		{
			DOMAIN_ERROR( "negative array subscript" );
		}
		
		if ( index.number() > 0xFFFFFFFFu )
		{
			DOMAIN_ERROR( "Array subscript is too large" );
		}
		
		unsigned i = index.number().clipped();
		
		Value nth = get_nth( expr.right, i );
		
		if ( is_empty( nth ) )
		{
			DOMAIN_ERROR( "subscript exceeds array bounds" );
		}
		
		return nth;
	}
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		if ( right.type() == Value_dummy_operand )
		{
			return calc_unary( op, left );
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
		
		if ( left.type() == right.type() )
		{
			switch ( left.type() )
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
		
		if ( op == Op_subscript )
		{
			if ( Expr* expr = left.expr() )
			{
				if ( expr->op == Op_array )
				{
					return array_subscript( *expr, right );
				}
			}
			
			switch ( left.type() )
			{
				case Value_string:
					return string_subscript( left.string(), right );
				
				default:
					TYPE_ERROR( "type not subscriptable" );
			}
		}
		
		if ( op == Op_multiply  &&  left.type() == Value_string )
		{
			if ( right.type() == V_bool  ||  right.type() == V_int )
			{
				return repeat_string( get_str( left ), get_int( right ) );
			}
		}
		
		SYNTAX_ERROR( "operator not defined on mixed types" );
		
		return Value();
	}
	
}
