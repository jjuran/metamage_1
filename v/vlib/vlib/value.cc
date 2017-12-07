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
	
	const Value NIL           = Value_NIL;
	const Value nothing       = Value_nothing;
	const Value dummy_operand = Value_dummy_operand;
	const Value undefined     = Value_undefined;
	const Value empty_list    = Value_empty_list;
	const Value empty_array   = Value_empty_array;
	
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
		its_dispatch = NULL;
		new ((void*) its_box.pointer()) Expr( a, Op_list, b );
	}
	
	Value::Value( op_type op, const Value& v )
	:
		its_box( sizeof (Expr), &pair_destructor, Value_pair )
	{
		its_dispatch = NULL;
		new ((void*) its_box.pointer()) Expr( Value_dummy_operand, op, v );
	}
	
	Value::Value( const Value&        a,
	              op_type             op,
	              const Value&        b,
	              const source_spec&  s )
	:
		its_box( sizeof (Expr), &pair_destructor, Value_pair )
	{
		its_dispatch = NULL;
		new ((void*) its_box.pointer()) Expr( a, op, b, s );
	}
	
	Value::Value( const Value&        a,
	              op_type             op,
	              const Value&        b,
	              const dispatch*     d )
	:
		its_box( sizeof (Expr), &pair_destructor, Value_pair )
	{
		its_dispatch = d;
		new ((void*) its_box.pointer()) Expr( a, op, b );
	}
	
	Value::Value( long n, destructor dtor, value_type t, const dispatch* d )
	:
		its_box( n, dtor, t )
	{
		its_dispatch = d;
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
		
		swap( its_dispatch, that.its_dispatch );
		swap( its_box,      that.its_box      );
	}
	
	static
	void add_ref( vbox& box )
	{
		if ( box.has_extent() )
		{
			plus::extent_add_ref( (const char*) box.pointer() );
		}
	}
	
	const Value& Value::secret() const
	{
		its_box.secret();
		
		return *this;
	}
	
	Value& Value::unshare()
	{
		if ( its_box.refcount() > 1 )
		{
			if ( Expr* exp = expr() )
			{
				add_ref( exp->left .its_box );
				add_ref( exp->right.its_box );
				
				typedef const plus::datum_storage string_datum;
				
				string_datum& datum = (string_datum&) exp->source.file();
				
				if ( plus::has_dynamic_extent( datum ) )
				{
					plus::extent_add_ref( datum.alloc.pointer );
				}
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
	
	unsigned long area( const Value& v )
	{
		unsigned long total = area( v.its_box ) + sizeof (dispatch*);
		
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
	
	Value& first_mutable( Value& list )
	{
		list.unshare();
		
		if ( Expr* expr = list.listexpr() )
		{
			return expr->left;
		}
		
		return list;  // not a (non-empty) list
	}
	
	Value& rest_mutable( Value& list )
	{
		list.unshare();
		
		if ( Expr* expr = list.listexpr() )
		{
			return expr->right;
		}
		
		throw mutable_list_overrun();
	}
	
}
