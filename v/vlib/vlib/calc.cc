/*
	calc.cc
	-------
*/

#include "vlib/calc.hh"

// relix-compat
#include "relix/recurse.hh"

// debug
#include "debug/assert.hh"

// plus
#include "plus/integer.hh"
#include "plus/reverse.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/error.hh"
#include "vlib/exceptions.hh"
#include "vlib/list-utils.hh"
#include "vlib/os.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/types.hh"
#include "vlib/type_info.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	using math::integer::cmp_t;
	
	
	static
	const plus::integer& nonzero( const plus::integer& x )
	{
		if ( x.is_zero() )
		{
			THROW( "division by zero" );
		}
		
		return x;
	}
	
	static
	bool isa( const Value& v, const Value& t )
	{
		return as_assigned( t, v ).type();
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
				return a.number() == b.number();
			
			case Value_data:
			case Value_string:
				return a.string() == b.string();
			
			case Value_function:
				return &a.proc() == &b.proc();
			
			case Value_base_type:
				return &a.typeinfo() == &b.typeinfo();
			
			case Value_pair:
				{
					Expr& ax = *a.expr();
					Expr& bx = *b.expr();
					
					if ( ax.op != bx.op )
					{
						return false;
					}
					
					const bool compare_left = (ax.op & 0xFF) != Op_scope;
					
					if ( compare_left  &&  ! equal_atoms( ax.left, bx.left ) )
					{
						return false;
					}
					
					return equal_atoms( ax.right, bx.right );
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
			THROW( "mismatched types in equality relation" );
		}
		
		list_iterator a( one );
		list_iterator b( two );
		
		while ( a  ||  b )
		{
			if ( ! equal_atoms( a.use(), b.use() ) )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static
	cmp_t compare( const Value& a, const Value& b )
	{
		if ( a.type() != b.type() )
		{
			THROW( "mismatched types in compare()" );
		}
		
		switch ( a.type() )
		{
			case Value_byte:
			case Value_number:
				return compare( a.number(), b.number() );
			
			case Value_data:
			case Value_string:
				return compare( a.string(), b.string() );
			
			default:
				THROW( "unsupported type in compare()" );
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
		
		Value result = Byte( *--it );
		
		while ( it > begin )
		{
			result = Value( Byte( *--it ), result );
		}
		
		return result;
	}
	
	static
	Value expand_range( const Value& range )
	{
		Expr* expr = range.expr();
		
		const plus::integer& left  = expr->left.number();
		const plus::integer& right = expr->right.number();
		
		plus::integer i = right;
		
		if ( expr->op == Op_delta )
		{
			--i;
		}
		
		if ( left > i )
		{
			return empty_list;
		}
		
		Value result = Integer( i );
		
		while ( --i >= left )
		{
			result = Value( Integer( i ), result );
		}
		
		return result;
	}
	
	static
	Value reversed_bytes( Value v )
	{
		v.unshare();
		
		const plus::string& s = v.string();
		
		plus::reverse( (char*) s.data(), s.size() );
		
		return v;
	}
	
	static
	Value calc_unary( op_type op, const Value& v )
	{
		if ( op == Op_array  ||  op == Op_block )
		{
			return Value( op, v );
		}
		
		if ( op == Op_unary_count )
		{
			return Integer( count( v ) );
		}
		
		if ( op == Op_return )
		{
			throw transfer_via_return( v, source_spec() );
		}
		
		if ( op == Op_throw )
		{
			throw user_exception( v, source_spec() );
		}
		
		if ( Expr* expr = v.expr() )
		{
			switch ( expr->op )
			{
				case Op_list:
					THROW( "unary operator not defined for lists" );
				
				case Op_array:
					if ( op == Op_unary_deref )
					{
						return expr->right;
					}
					
					if ( op == Op_unary_minus )
					{
						return make_array( reverse_list( expr->right ) );
					}
					
					THROW( "unary operator not defined for arrays" );
				
				case Op_gamut:
				case Op_delta:
					if ( op == Op_unary_deref )
					{
						return expand_range( v );
					}
					
					if ( op == Op_unary_minus )
					{
						return make_array( reverse_list( expand_range( v ) ) );
					}
					
					THROW( "unary operator not defined for ranges" );
				
				case Op_invocation:
					THROW( "unary operator not defined for blocks" );
				
				default:
					INTERNAL_ERROR( "unary operator not defined for pairs" );
			}
		}
		
		switch ( v.type() )
		{
			case Value_empty_list:
				return Integer();
			
			case Value_boolean:
				switch ( op )
				{
					case Op_not:  return Bool( ! v.boolean() );
					
					case Op_unary_plus:   return Integer(  v.boolean() );
					case Op_unary_minus:  return Integer( -v.boolean() );
					
					default:  break;
				}
				
				THROW( "unary operator not defined for booleans" );
				break;
			
			case Value_number:
				switch ( op )
				{
					case Op_unary_plus:   return v;
					case Op_unary_minus:  return Integer( -v.number() );
					
					default:  break;
				}
				
				THROW( "unary operator not defined for integers" );
				break;
			
			case Value_data:
				if ( op == Op_unary_deref  ||  op == Op_unary_minus )
				{
					// fall through below
				}
				else
				{
					THROW( "unary operator not defined for raw data" );
				}
				
				// fall through
			
			case Value_string:
				if ( op == Op_unary_deref )
				{
					return deref_string( v.string() );
				}
				
				if ( op == Op_unary_minus )
				{
					return reversed_bytes( v );
				}
				
				THROW( "unary operator not defined for string values" );
				break;
			
			case Value_base_type:
				THROW( "unary operator not defined for types" );
				break;
			
			case Value_function:
				THROW( "unary operator not defined for functions" );
				break;
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in calc_unary()" );
		
		return Value();
	}
	
	static
	Value array_member( const Value& array, const plus::string& name )
	{
		Expr* expr = array.expr();
		
		ASSERT( expr != NULL );
		ASSERT( expr->op == Op_array );
		
		if ( name == "length" )
		{
			return Integer( count( expr->right ) );
		}
		
		THROW( "nonexistent array member" );
		
		return Value();
	}
	
	static
	Value mapping_member( const Value& mapping, const plus::string& name )
	{
		Expr* expr = mapping.expr();
		
		ASSERT( expr != NULL );
		ASSERT( expr->op == Op_mapping );
		
		if ( name == "key" )
		{
			return expr->left;
		}
		
		if ( name == "value" )
		{
			return expr->right;
		}
		
		THROW( "nonexistent mapping member" );
		
		return Value();
	}
	
	static
	Value calc_member( const Value& left, const Value& right )
	{
		if ( right.type() != Value_string )
		{
			THROW( "non-string member name" );
		}
		
		Value vtype = proc_typeof.addr( left );
		
		if ( Expr* expr = vtype.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				return array_member( left, right.string() );
			}
			
			if ( expr->op == Op_mapping )
			{
				return mapping_member( left, right.string() );
			}
		}
		
		if ( vtype.type() == Value_base_type )
		{
			const type_info& typeinfo = vtype.typeinfo();
			
			if ( member_proc member = typeinfo.member )
			{
				return member( left, right.string() );
			}
		}
		
		THROW( "unsupported type for member access" );
		
		return Value();
	}
	
	static
	plus::integer calc( const plus::integer&  left,
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
		
		return 0;
	}
	
	static
	plus::string repeat_string( const plus::string& s, const plus::integer& n )
	{
		typedef plus::string::size_type size_t;
		
		if ( n.is_negative() )
		{
			THROW( "negative string multiplier" );
		}
		
		if ( n > size_t( -1 ) )
		{
			THROW( "excessively large string multiplier" );
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
			THROW( "non-numeric list repetition factor" );
		}
		
		if ( factor.number().is_negative() )
		{
			THROW( "negative list repetition factor" );
		}
		
		if ( factor.number().is_zero()  ||  is_empty( list ) )
		{
			return Value_empty_list;
		}
		
		if ( factor.number() > 0xFFFFFFFFu )
		{
			THROW( "excessively large list multiplier" );
		}
		
		unsigned long n = factor.number().clipped();
		
		if ( n == 1 )
		{
			return list;
		}
		
		Value result = list;
		
		while ( --n > 0 )
		{
			result = make_list( list, result );
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
	bool is_etc( const Value& type )
	{
		if ( type.type() == Value_base_type )
		{
			const type_info& typeinfo = type.typeinfo();
			
			return &typeinfo == &etc_vtype;
		}
		
		return false;
	}
	
	static
	Value as_assigned_or_default( const Value& type, const Value& v )
	{
		if ( Expr* expr = type.expr() )
		{
			if ( expr->op == Op_duplicate )
			{
				if ( is_empty( v ) )
				{
					ASSERT( as_assigned( expr->left, expr->right ).type() );
					
					return expr->right;
				}
				
				return as_assigned( expr->left, v );
			}
		}
		
		return as_assigned( type, v );
	}
	
	static
	Value apply_prototype( const Value& prototype, const Value& arguments )
	{
		Value result = Value_empty_list;
		
		list_iterator defs( prototype );
		list_iterator args( arguments );
		
		while ( defs )
		{
			const Value& type = defs.use();
			
			if ( is_etc( type ) )
			{
				if ( defs )
				{
					THROW( "`...` must be last in a prototype" );
				}
				
				result = calc( result, Op_list, args.rest() );
				
				return result;
			}
			
			const Value& arg = args.use();
			
			const Value r = as_assigned_or_default( type, arg );
			
			if ( r.type() == Value_nothing )
			{
				THROW( "arguments don't match function prototype" );
			}
			
			result = calc( result, Op_list, r );
		}
		
		if ( args )
		{
			THROW( "too many arguments" );
		}
		
		return result;
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
		if ( f.type() == V_str )
		{
			return str( Value( f, arguments ) );
		}
		
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
			
			THROW( "invalid type conversion arguments" );
		}
		
		if ( Expr* expr = f.expr() )
		{
			if ( expr->op == Op_lambda )
			{
				try
				{
					return call_function( expr->right, arguments );
				}
				catch ( const transfer_via_return& e )
				{
					return e.object;
				}
				catch ( const transfer_via_break& e )
				{
					THROW( "`break` used outside of loop" );
				}
				catch ( const transfer_via_continue& e )
				{
					THROW( "`continue` used outside of loop" );
				}
			}
			
			if ( expr->op == Op_invocation )
			{
				const Value& invoke = expr->left;
				const Value& block  = expr->right;
				
				Value bound_block( block, Op_bind_args, arguments );
				
				return recurse( invoke.proc().addr, bound_block );
			}
			
			const Value& method = expr->left;
			const Value& object = expr->right;
			
			return call_function( method, make_list( object, arguments ) );
		}
		
		THROW( "attempted call of non-function" );
		
		return Value();  // not reached
	}
	
	static
	Value map( const Value& array, const Value& f )
	{
		Value result = Value_empty_list;
		
		Expr* expr = array.expr();
		
		if ( expr == NULL  ||  expr->op != Op_array )
		{
			THROW( "map requires an array" );
		}
		
		if ( ! is_functional( f ) )
		{
			THROW( "map requires a function" );
		}
		
		list_iterator it( expr->right );
		
		while ( it )
		{
			const Value& x = it.use();
			
			Value f_x = call_function( f, x );
			
			result = make_list( result, f_x );
		}
		
		return make_array( result );
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
			
			THROW( "`if ... then ... else` requires blocks" );
		}
		
		THROW( "`if ... then` requires a block" );
		
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
		
		THROW( "`while ... do` requires a block" );
		
		return loop_block_set();
	}
	
	static
	Value calc_if( const Value& then )
	{
		Expr* expr = then.expr();
		
		if ( expr == NULL  ||  expr->op != Op_then )
		{
			THROW( "`if` requires `then`" );
		}
		
		conditional_resolution resolution = resolution_for_if( expr->right );
		
		const bool condition = boolean_vtype.coerce( expr->left ).boolean();
		
		if ( condition )
		{
			return do_block( *resolution.affirm );
		}
		else if ( resolution.negate != NULL )
		{
			return do_block( *resolution.negate );
		}
		
		return Value_empty_list;
	}
	
	static
	Value calc_while( const Value& _do_ )
	{
		Expr* expr = _do_.expr();
		
		if ( expr == NULL  ||  expr->op != Op_do_2 )
		{
			THROW( "`while` requires `do`" );
		}
		
		loop_block_set blocks = blocks_for_while( expr->right );
		
		Value result;
		
		while ( boolean_vtype.coerce( do_block( expr->left ) ).boolean() )
		{
			periodic_yield();
			
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
		}
		
		return result;
	}
	
	static
	Value calc_do( const Value& block )
	{
		if ( is_block( block ) )
		{
			return do_block( block );
		}
		
		Expr* expr = block.expr();
		
		if ( expr == NULL  ||  expr->op != Op_while_2 )
		{
			THROW( "`do` requires a block" );
		}
		
		Value result;
		
		do
		{
			periodic_yield();
			
			try
			{
				result = do_block( expr->left );
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
		}
		while ( boolean_vtype.coerce( do_block( expr->right ) ).boolean() );
		
		return result;
	}
	
	static
	Value calc_try( const Value& operand )
	{
		Expr* expr = operand.expr();
		
		if ( expr == NULL  ||  expr->op != Op_catch )
		{
			THROW( "`try` requires catch" );
		}
		
		const Value& attempt = expr->left;
		const Value& catcher = expr->right;
		
		if ( ! is_block( attempt ) )
		{
			THROW( "`try` requires a block" );
		}
		
		if ( ! is_block( catcher ) )
		{
			THROW( "`catch` requires a block" );
		}
		
		try
		{
			return do_block( attempt );
		}
		catch ( const user_exception& e )
		{
			return call_function( catcher, e.object );
		}
	}
	
	static
	void check_range_operand( const Value& point )
	{
		if ( point.type() != Value_number )
		{
			THROW( "non-integer range operand" );
		}
	}
	
	static
	Value safe_calc( const Value&  left,
	                 op_type       op,
	                 const Value&  right );
	
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
		
		if ( op == Op_do )
		{
			return calc_do( left );
		}
		
		if ( op == Op_break )
		{
			throw transfer_via_break( source_spec() );
		}
		
		if ( op == Op_continue )
		{
			throw transfer_via_continue( source_spec() );
		}
		
		if ( op == Op_try )
		{
			return calc_try( left );
		}
		
		if ( op == Op_function  ||  op == Op_named_unary )
		{
			return call_function( left, right );
		}
		
		if ( op == Op_map )
		{
			return map( left, right );
		}
		
		return safe_calc( left, op, right );
	}
	
	Value safe_calc( const Value&  left,
	                 op_type       op,
	                 const Value&  right )
	{
		if ( op == Op_lambda )
		{
			if ( ! is_block( left ) )
			{
				THROW( "`lambda` requires a block" );
			}
			
			return Value( Op_lambda, left );
		}
		
		if ( right.type() == Value_dummy_operand )
		{
			return calc_unary( op, left );
		}
		
		switch ( op )
		{
			case Op_member:
				return calc_member( left, right );
			
			case Op_gamut:
			case Op_delta:
				check_range_operand( left  );
				check_range_operand( right );
				
				goto no_op;
			
			case Op_isa:      return Bool(   isa  ( left, right ) );
			case Op_equal:    return Bool(   equal( left, right ) );
			case Op_unequal:  return Bool( ! equal( left, right ) );
			
			case Op_lt:   return Bool( compare( left, right ) <  0 );
			case Op_lte:  return Bool( compare( left, right ) <= 0 );
			case Op_gt:   return Bool( compare( left, right ) >  0 );
			case Op_gte:  return Bool( compare( left, right ) >= 0 );
			
			case Op_cmp:  return Integer( compare( left, right ) );
			
			default:  break;
		}
		
		if ( op == Op_list )
		{
			return make_list( left, right );
		}
		
		if ( is_supporting( op ) )
		{
			goto no_op;
		}
		
		if ( op == Op_subscript )
		{
			if ( is_type( left )  &&  is_empty( right ) )
			{
				goto no_op;
			}
		}
		
		if ( op == Op_intersection  ||  op == Op_union )
		{
			if ( is_type( left )  &&  is_type( right ) )
			{
				goto no_op;
			}
		}
		
		if ( op == Op_repeat )
		{
			return repeat_list( left, right );
		}
		
		if ( op == Op_format  &&  left.type() == Value_string )
		{
			return format( left.string(), right );
		}
		
		if ( op == Op_bind_args  &&  is_functional( left ) )
		{
			return bind_args( left, right );
		}
		
		if ( op == Op_subscript )
		{
			const Value v = linear_subscript( left, right );
			
			if ( Expr* expr = right.expr() )
			{
				switch ( left.type() )
				{
					case Value_data:    return pack( v );
					case Value_string:  return str ( v );
					
					default:
						break;
				}
			}
			
			return v;
		}
		
		if ( left.type() == right.type() )
		{
			switch ( left.type() )
			{
				case Value_empty_list:
					THROW( "operator not defined for empty list" );
				
				case Value_boolean:
					THROW( "operator not defined for boolean values" );
				
				case Value_number:
					try
					{
						const plus::integer& a = left.number();
						const plus::integer& b = right.number();
						
						return Integer( calc( a, op, b ) );
					}
					catch ( const plus::ibox::limb_count_overflow& )
					{
						THROW( "bigint overflow" );
					}
					// not reached
				
				case Value_string:
					THROW( "operator not defined for string values" );
				
				case Value_function:
					THROW( "operator not defined for functions" );
				
				case Value_base_type:
					THROW( "operator not defined for types" );
				
				case Value_pair:
					THROW( "operator not defined for lists" );
				
				default:
					break;
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
				
				return repeat_string( left.string(), right.number() );
			}
		}
		
		THROW( "operator not defined on mixed types" );
		
	no_op:
		
		return Value( left, op, right );
	}
	
}
