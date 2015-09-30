/*
	value.cc
	--------
*/

#include "vlib/value.hh"

// Standard C++
#include <new>

// iota
#include "iota/swap.hh"

// plus
#include "plus/extent.hh"


namespace vlib
{
	
	const Value empty_list = Value_empty_list;
	
	void pair_destructor( void* pointer )
	{
		Expr* expr = (Expr*) pointer;
		
		while ( expr->op )
		{
			Value& next = const_cast< Value& >( expr->right );
			
			Expr* next_extent = NULL;
			
			if ( next.type() == Value_pair )
			{
				next_extent = (Expr*) next.its_box.transfer_extent();
			}
			
			expr->~Expr();
			
			if ( expr != pointer )
			{
				plus::extent_release( (char*) expr );
			}
			
			if ( next_extent == NULL )
			{
				break;
			}
			
			expr = next_extent;
		}
	}
	
	Value::Value( const Value& a, const Value& b )
	:
		its_box( sizeof (Expr), &pair_destructor, Value_pair )
	{
		new ((void*) its_box.pointer()) Expr( a, Op_list, b );
	}
	
	Value::Value( op_type op, const Value& v )
	:
		its_box( sizeof (Expr), &pair_destructor, Value_pair )
	{
		new ((void*) its_box.pointer()) Expr( Value_dummy_operand, op, v );
	}
	
	Value::Value( const Value& a, op_type op, const Value& b )
	:
		its_box( sizeof (Expr), &pair_destructor, Value_pair )
	{
		new ((void*) its_box.pointer()) Expr( a, op, b );
	}
	
	Expr* Value::listexpr() const
	{
		if ( Expr* exp = expr() )
		{
			if ( exp->op == Op_list )
			{
				return exp;
			}
		}
		
		return 0;  // NULL
	}
	
	void Value::swap( Value& that )
	{
		using iota::swap;
		
		swap( its_box, that.its_box );
	}
	
	Expr::Expr( const Value& a, op_type op, const Value& b )
	:
		op( op ),
		left( a ),
		right( b )
	{
	}
	
	Value bind_args( const Value& f, const Value& arguments )
	{
		if ( is_empty( arguments ) )
		{
			return f;
		}
		
		Expr* expr = get_expr( f );
		
		if ( expr != NULL  &&  expr->op == Op_bind_args )
		{
			return bind_args( expr->left, Value( expr->right, arguments ) );
		}
		
		return Value( f, Op_bind_args, arguments );
	}
	
	unsigned long area( const Value& v )
	{
		unsigned long total = area( v.its_box );
		
		if ( v.type() != Value_pair )
		{
			return total;
		}
		
		Expr* expr = v.expr();
		
		while ( expr != NULL )
		{
			total -= 2 * sizeof (Value);
			
			total += area( expr->left );
			total += area( expr->right.its_box );
			
			if ( expr->right.type() != Value_pair )
			{
				break;
			}
			
			expr = expr->right.expr();
		}
		
		return total;
	}
	
}
