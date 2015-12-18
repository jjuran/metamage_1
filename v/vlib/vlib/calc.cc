/*
	calc.cc
	-------
*/

#include "vlib/calc.hh"

// relix-compat
#include "relix/recurse.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/list-utils.hh"
#include "vlib/os.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
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
				return a.boolean() == b.boolean();
			
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
				
				case Op_invocation:
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
				switch ( op )
				{
					case Op_unary_plus:   return 0 + v.boolean();
					case Op_unary_minus:  return 0 - v.boolean();
					
					default:  break;
				}
				
				SYNTAX_ERROR( "unary operator not defined for booleans" );
				break;
			
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
			
			case Value_base_type:
				SYNTAX_ERROR( "unary operator not defined for types" );
				break;
			
			case Value_function:
				SYNTAX_ERROR( "unary operator not defined for functions" );
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
		if ( factor.type() == Value_boolean )
		{
			return factor.boolean() ? list : Value_empty_list;
		}
		
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
	Value do_block( const Value& block )
	{
		Expr* expr = block.expr();
		
		if ( expr == NULL  ||  expr->op != Op_invocation )
		{
			INTERNAL_ERROR( "not a block invocation" );
		}
		
		const Value& invoke = expr->left;
		const Value& body   = expr->right;
		
		return invoke.proc().addr( body );
	}
	
	static
	void recursive_call( function_type invoke, const Value& f, Value& result )
	{
		result = invoke( f );
	}
	
	static
	Value recurse( function_type f, const Value& v )
	{
		using relix::recurse;
		
		typedef function_type  F;
		typedef const Value&   V;
		typedef       Value&   R;
		
		Value result;
		
		recurse< void, F, F, V, V, R, R >( &recursive_call, f, v, result );
		
		return result;
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
			if ( expr->op == Op_invocation )
			{
				const Value& invoke = expr->left;
				const Value& block  = expr->right;
				
				Value bound_block( block, Op_bind_args, arguments );
				
				return recurse( invoke.proc().addr, bound_block );
			}
			
			const Value& method = expr->left;
			const Value& object = expr->right;
			
			return call_function( method, calc( object, Op_list, arguments ) );
		}
		
		TYPE_ERROR( "attempted call of non-function" );
		
		return Value();  // not reached
	}
	
	static
	Value map( const Value& array, const Value& f )
	{
		Value result = Value_empty_list;
		
		Expr* expr = array.expr();
		
		if ( expr == NULL  ||  expr->op != Op_array )
		{
			TYPE_ERROR( "map requires an array" );
		}
		
		if ( ! is_functional( f ) )
		{
			TYPE_ERROR( "map requires a function" );
		}
		
		const Value* next = &expr->right;
		
		while ( ! is_empty( *next ) )
		{
			const Value& x = first( *next );
			
			if ( is_empty( x ) )
			{
				break;
			}
			
			Value f_x = call_function( f, x );
			
			result = calc( result, Op_list, f_x );
			
			next = &rest( *next );
		}
		
		return make_array( result );
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
		
		const Value& nth = get_nth( expr.right, i );
		
		if ( is_empty( nth ) )
		{
			DOMAIN_ERROR( "subscript exceeds array bounds" );
		}
		
		return nth;
	}
	
	struct conditional_resolution
	{
		const Value* affirm;
		const Value* negate;
	};
	
	struct loop_block_set
	{
		const Value* loop;  // the loop body
	};
	
	static inline
	bool is_block( const Value& v )
	{
		Expr* expr = v.expr();
		
		return expr != NULL  &&  expr->op == Op_invocation;
	}
	
	static
	conditional_resolution resolution_for_if( const Value& extension )
	{
		if ( is_block( extension ) )
		{
			conditional_resolution result = { &extension, NULL };
			
			return result;
		}
		
		Expr* expr = extension.expr();
		
		if ( expr != NULL  &&  expr->op == Op_else )
		{
			const Value& affirm = expr->left;
			const Value& negate = expr->right;
			
			if ( is_block( affirm )  &&  is_block( negate ) )
			{
				conditional_resolution result = { &affirm, &negate };
				
				return result;
			}
			
			SYNTAX_ERROR( "`if ... then ... else` requires blocks" );
		}
		
		SYNTAX_ERROR( "`if ... then` requires a block" );
		
		return conditional_resolution();
	}
	
	static
	loop_block_set blocks_for_while( const Value& extension )
	{
		if ( is_block( extension ) )
		{
			loop_block_set result = { &extension };
			
			return result;
		}
		
		SYNTAX_ERROR( "`while ... do` requires a block" );
		
		return loop_block_set();
	}
	
	static
	Value calc_if( const Value& then )
	{
		Expr* expr = then.expr();
		
		if ( expr == NULL  ||  expr->op != Op_then )
		{
			SYNTAX_ERROR( "`if` requires `then`" );
		}
		
		conditional_resolution resolution = resolution_for_if( expr->right );
		
		const bool condition = get_bool( boolean_vtype.coerce( expr->left ) );
		
		if ( condition )
		{
			return do_block( *resolution.affirm );
		}
		else if ( resolution.negate != NULL )
		{
			return do_block( *resolution.negate );
		}
		
		return Value_nothing;
	}
	
	static
	Value calc_while( const Value& _do_ )
	{
		Expr* expr = _do_.expr();
		
		if ( expr == NULL  ||  expr->op != Op_do )
		{
			SYNTAX_ERROR( "`while` requires `do`" );
		}
		
		loop_block_set blocks = blocks_for_while( expr->right );
		
		Value result;
		
		while ( get_bool( boolean_vtype.coerce( do_block( expr->left ) ) ) )
		{
			try
			{
				result = do_block( *blocks.loop );
			}
			catch ( const transfer_via_break& )
			{
				return Value_nothing;
			}
			catch ( const transfer_via_continue& )
			{
				result = Value_nothing;
				
				continue;
			}
			
			periodic_yield();
		}
		
		return result;
	}
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		if ( op == Op_if )
		{
			return calc_if( left );
		}
		
		if ( op == Op_while )
		{
			return calc_while( left );
		}
		
		if ( op == Op_break )
		{
			throw transfer_via_break();
		}
		
		if ( op == Op_continue )
		{
			throw transfer_via_continue();
		}
		
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
			
			case Op_map:      return map( left, right );
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
		
		if ( is_supporting( op ) )
		{
			return Value( left, op, right );
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
				if ( right.type() == V_bool )
				{
					return right.boolean() ? left.string() : plus::string::null;
				}
				
				return repeat_string( get_str( left ), get_int( right ) );
			}
		}
		
		SYNTAX_ERROR( "operator not defined on mixed types" );
		
		return Value();
	}
	
}
