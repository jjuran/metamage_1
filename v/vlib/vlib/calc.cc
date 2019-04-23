/*
	calc.cc
	-------
*/

#include "vlib/calc.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/assign.hh"
#include "vlib/compare.hh"
#include "vlib/equal.hh"
#include "vlib/error.hh"
#include "vlib/exceptions.hh"
#include "vlib/function-utils.hh"
#include "vlib/is_type.hh"
#include "vlib/list-utils.hh"
#include "vlib/map-reduce.hh"
#include "vlib/os.hh"
#include "vlib/return.hh"
#include "vlib/string-utils.hh"
#include "vlib/table-utils.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/types.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/generic_iterator.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/byterange.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/iterator.hh"
#include "vlib/types/lambda.hh"
#include "vlib/types/null.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/range.hh"
#include "vlib/types/table.hh"
#include "vlib/types/vbytes.hh"


namespace vlib
{
	
	static inline
	bool isa( const Value& v, const Value& t )
	{
		return as_assigned( t, v ).type();
	}
	
	static
	Value generic_deref( const Value& v )
	{
		if ( is_type( v ) )
		{
			return Value( Op_unary_deref, v );
		}
		
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_array )
			{
				return expr->right;
			}
			
			if ( expr->op == Op_empower )
			{
				return generic_deref( expr->right );
			}
			
			if ( expr->op == Op_unary_refer )
			{
				Target target = make_target( expr->right );
				
				if ( is_undefined( *target.addr ) )
				{
					THROW( "undefined value in dereference" );
				}
				
				return *target.addr;
			}
		}
		
		generic_iterator it( v );
		
		list_builder result;
		
		while ( it )
		{
			result.append( it.use() );
		}
		
		return result;
	}
	
	static
	bool in_list( const Value& v, const Value& list )
	{
		list_iterator it( list );
		
		while ( it )
		{
			if ( equal( v, it.use() ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	static
	bool in_array_mapping_keys( const Value& v, const Value& array )
	{
		array_iterator it( array );
		
		while ( it )
		{
			const Value& mapping = it.use();
			const Value& key = mapping.expr()->left;
			
			if ( equal( key, v ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	static
	bool in( const Value& v, const Value& container )
	{
		if ( Expr* expr = container.expr() )
		{
			switch ( expr->op )
			{
				case Op_gamut:
				case Op_delta:
					return compare( v, expr->left  ) >= 0  &&
					       compare( v, expr->right ) < (expr->op == Op_gamut);
				
				case Op_array:
					return in_list( v, expr->right );
				
				case Op_empower:
					return in_array_mapping_keys( v, expr->right );
				
				default:
					break;
			}
		}
		
		switch ( container.type() )
		{
			case Value_empty_array:
				return false;
			
			default:
				break;
		}
		
		if ( Value contains = calc( container, Op_contains, v ) )
		{
			return contains.as< Boolean >();
		}
		
		THROW( "unsupported container type for `in`" );
		return false;
	}
	
	static
	Value calc_unary( op_type op, const Value& v )
	{
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_1arg handler = ops->unary )
				{
					const Value result = handler( op, v );
					
					if ( result )
					{
						return result;
					}
				}
			}
		}
		
		if ( op == Op_array  &&  is_empty_list( v ) )
		{
			return empty_array;
		}
		
		if ( op == Op_array  ||  op == Op_block )
		{
			return Value( op, v );
		}
		
		if ( op == Op_unary_count )
		{
			return Integer( count( v ) );
		}
		
		if ( op == Op_unary_deref )
		{
			return generic_deref( v );
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
					if ( op == Op_unary_minus )
					{
						return make_array( reverse_list( expr->right ) );
					}
					
					if ( op == Op_begin )
					{
						return Iterator( expr->right );
					}
					
					THROW( "unary operator not defined for arrays" );
				
				case Op_empower:
					if ( op == Op_unary_minus )
					{
						const Value& old_list = expr->right.expr()->right;
						
						const Value new_list  = reverse_list( old_list );
						const Value new_array = make_array  ( new_list );
						
						return Value( expr->left, expr->op, new_array );
					}
					
					THROW( "unary operator not defined for arrays" );
				
				case Op_gamut:
				case Op_delta:
					if ( op == Op_unary_minus )
					{
						return make_array( reverse_list( generic_deref( v ) ) );
					}
					
					THROW( "unary operator not defined for ranges" );
				
				case Op_invocation:
					THROW( "unary operator not defined for blocks" );
				
				case Op_begin:
					THROW( "unary operator not defined for iterators" );
				
				default:
					INTERNAL_ERROR( "unary operator not defined for pairs" );
			}
		}
		
		switch ( v.type() )
		{
			case Value_empty_list:
				if ( op == Op_begin )
				{
					return Iterator( empty_list );
				}
				
				return Integer();
			
			case Value_empty_array:
				if ( op == Op_unary_minus )
				{
					return v;
				}
				
				if ( op == Op_begin )
				{
					return Iterator( empty_list );
				}
				
				THROW( "unary operator not defined for (empty) arrays" );
				break;
			
			
			case Value_boolean:
				THROW( "unary operator not defined for booleans" );
				break;
			
			case Value_number:
				THROW( "unary operator not defined for integers" );
				break;
			
			case Value_packed:
				THROW( "unary operator not defined for packed values" );
			
			case Value_string:
				THROW( "unary operator not defined for string values" );
				break;
			
			case Value_base_type:
				THROW( "unary operator not defined for types" );
				break;
			
			default:
				break;
		}
		
		THROW( "unary operator not implemented by operand" );
		
		return Value();
	}
	
	static
	Value array_member( const Value& array, const plus::string& name )
	{
		if ( is_empty_array( array ) )
		{
			if ( name == "length" )
			{
				return Integer();
			}
			
			THROW( "nonexistent array member" );
		}
		
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
	Value table_member( const Value& table, const plus::string& name )
	{
		const Value& array = table.expr()->right;
		
		if ( name == "length" )
		{
			return array_member( array, name );
		}
		
		if ( name == "keys"  ||  name == "values" )
		{
			if ( is_empty_array( array ) )
			{
				return array;
			}
			
			const bool keys = name == "keys";
			
			list_builder results;
			
			array_iterator it( array );
			
			while ( it )
			{
				const Value& mapping = it.use();
				
				Expr* expr = mapping.expr();
				
				results.append( keys ? expr->left : expr->right );
			}
			
			return make_array( results );
		}
		
		THROW( "nonexistent table member" );
		
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
			
			if ( expr->op == Op_empower )
			{
				return table_member( left, right.string() );
			}
			
			if ( expr->op == Op_module )
			{
				// Pretend the module is a table.
				return associative_subscript( left, right );
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
	Value repeat_list( const Value& list, const Value& factor )
	{
		if ( const Boolean* value = factor.is< Boolean >() )
		{
			return *value ? list : Value_empty_list;
		}
		
		if ( is_empty_list( list ) )
		{
			return Value_empty_list;
		}
		
		unsigned long n = integer_cast< unsigned long >( factor );
		
		if ( n == 0 )
		{
			return Value_empty_list;
		}
		
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
		
		return invoke.as< Proc >().call( body );
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
		
		const bool condition = expr->left.to< Boolean >();
		
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
		
		while ( do_block( expr->left ).to< Boolean >() )
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
		while ( do_block( expr->right ).to< Boolean >() );
		
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
	Value safe_calc( const Value&  left,
	                 op_type       op,
	                 const Value&  right );
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		if ( const dispatch* methods = left.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_2arg handler = ops->binary )
				{
					const Value result = handler( op, left, right );
					
					if ( result )
					{
						return result;
					}
				}
			}
		}
		
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
		
		if ( op == Op_ver )
		{
			return filter( left, right );
		}
		
		if ( op == Op_per )
		{
			return reduce( left, right );
		}
		
		return safe_calc( left, op, right );
	}
	
	static inline
	bool is_bytes( const Value& v )
	{
		return v.type() == V_str  ||  v.type() == V_pack;
	}
	
	Value safe_calc( const Value&  left,
	                 op_type       op,
	                 const Value&  right )
	{
		if ( op == Op_lambda )
		{
			return Lambda( left );
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
				if ( left.is< Byte >()  &&  right.is< Byte >() )
				{
					return ByteRange( left, op, right );
				}
				
				return Range( left, op, right );
			
			case Op_in:       return Boolean(   in   ( left, right ) );
			case Op_isa:      return Boolean(   isa  ( left, right ) );
			case Op_equal:    return Boolean(   equal( left, right ) );
			case Op_unequal:  return Boolean( ! equal( left, right ) );
			
			case Op_lt:   return Boolean( compare( left, right ) <  0 );
			case Op_lte:  return Boolean( compare( left, right ) <= 0 );
			case Op_gt:   return Boolean( compare( left, right ) >  0 );
			case Op_gte:  return Boolean( compare( left, right ) >= 0 );
			
			case Op_cmp:  return Integer( compare( left, right ) );
			
			default:  break;
		}
		
		if ( op == Op_list )
		{
			return make_list( left, right );
		}
		
		if ( op == Op_mapping )
		{
			if ( ! is_single( left )  ||  left.is< Null >() )
			{
				THROW( "mapping keys must be singular and non-null" );
			}
			
			if ( ! is_single( right ) )
			{
				THROW( "mapping values must be singular" );
			}
		}
		
		if ( is_supporting( op ) )
		{
			goto no_op;
		}
		
		if ( op == Op_subscript )
		{
			if ( is_type( left )  &&  is_empty_list( right ) )
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
		
		if ( op == Op_empower )
		{
			if ( is_type( left ) )
			{
				if ( is_array( right ) )
				{
					return Table( left, right );
				}
				
				if ( is_type( right ) )
				{
					return Value( left, op, right );
				}
			}
		}
		
		if ( op == Op_repeat )
		{
			return repeat_list( left, right );
		}
		
		if ( op == Op_bind_args  &&  is_functional( left ) )
		{
			return bind_args( left, right );
		}
		
		if ( op == Op_subscript )
		{
			if ( is_table( left ) )
			{
				return associative_subscript( left, right );
			}
			
			const Value v = linear_subscript( left, right );
			
			if ( is_empty_array( right )  ||  right.expr() != 0 )  // NULL
			{
				if ( is_bytes( left ) )
				{
					return VBytes( left.type() == V_str ? str ( v )
					                                    : pack( v ),
					               left.type(),
					               left.dispatch_methods() );
				}
			}
			
			return v;
		}
		
		if ( op == Op_multiply )
		{
			if ( is_functional( left )  &&  is_functional( right ) )
			{
				goto no_op;
			}
		}
		
		if ( op == Op_empower )
		{
			if ( is_functional( left )  &&  right.is< Integer >() )
			{
				goto no_op;
			}
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
					THROW( "operator not defined for integer values" );
				
				case Value_packed:
					THROW( "operator not defined for packed values" );
				
				case Value_string:
					THROW( "operator not defined for string values" );
				
				case Value_base_type:
					THROW( "operator not defined for types" );
				
				case Value_pair:
					THROW( "operator not defined for lists" );
				
				default:
					break;
			}
		}
		
		THROW( "operator not defined on mixed types" );
		
	no_op:
		
		return Value( left, op, right );
	}
	
}
