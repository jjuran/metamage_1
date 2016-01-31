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

// vlib
#include "vlib/symbol.hh"  // codependent


namespace vlib
{
	
	const Value nothing       = Value_nothing;
	const Value dummy_operand = Value_dummy_operand;
	const Value undefined     = Value_undefined;
	const Value empty_list    = Value_empty_list;
	
	static
	void symbol_destructor( void* pointer )
	{
		Symbol* symbol = (Symbol*) pointer;
		
		symbol->~Symbol();
	}
	
	Value::Value( symbol_type symtype, const plus::string& name )
	:
		its_box( sizeof (Symbol), &symbol_destructor, Value_symbol )
	{
		new ((void*) its_box.pointer()) Symbol( symtype, name );
	}
	
	void pair_destructor( void* pointer )
	{
		Expr* expr = (Expr*) pointer;
		
		while ( expr->op )
		{
			Value& next = expr->right;
			
			Expr* next_extent = NULL;
			
			if ( next.type() == Value_pair )
			{
				next_extent = (Expr*) next.its_box.transfer_extent();
			}
			
			expr->~Expr();
			
			if ( expr != pointer )
			{
				// Don't call pair_destructor() recursively -- we iterate.
				plus::extent_set_destructor( (char*) expr, NULL );
				
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
	
	Value::Value( const Value&        a,
	              op_type             op,
	              const Value&        b,
	              const source_spec&  s )
	:
		its_box( sizeof (Expr), &pair_destructor, Value_pair )
	{
		new ((void*) its_box.pointer()) Expr( a, op, b, s );
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
	
	static
	void add_ref( vbox& box )
	{
		if ( box.has_extent() )
		{
			plus::extent_add_ref( (const char*) box.pointer() );
		}
	}
	
	Value& Value::unshare()
	{
		if ( its_box.refcount() > 1 )
		{
			if ( Expr* exp = expr() )
			{
				add_ref( exp->left .its_box );
				add_ref( exp->right.its_box );
			}
			
			its_box.unshare();
		}
		
		return *this;
	}
	
	Expr::Expr( const Value&        a,
	            op_type             op,
	            const Value&        b,
	            const source_spec&  s )
	:
		op( op ),
		left( a ),
		right( b ),
		source( s )
	{
	}
	
	Value bind_args( const Value& f, const Value& arguments )
	{
		if ( is_empty( arguments ) )
		{
			return f;
		}
		
		Expr* expr = f.expr();
		
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
	
	const Value& first( const Value& list )
	{
		if ( Expr* expr = list.listexpr() )
		{
			return expr->left;
		}
		
		return list;  // not a (non-empty) list
	}
	
	const Value& rest( const Value& list )
	{
		if ( Expr* expr = list.listexpr() )
		{
			return expr->right;
		}
		
		return empty_list;
	}
	
}
