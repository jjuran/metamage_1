/*
	iterator.cc
	-----------
*/

#include "vlib/iterator.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/symbol.hh"


namespace vlib
{
	
	full_iterator::full_iterator( const Value& start )
	{
		its_stack.push_back( start );
	}
	
	const Value& full_iterator::get() const
	{
		ASSERT( ! finished() );
		
		return its_stack.back();
	}
	
	void full_iterator::step()
	{
		ASSERT( ! finished() );
		
		const Value v = its_stack.back();
		
		its_stack.pop_back();
		
		if ( Expr* expr = v.expr() )
		{
			its_stack.push_back( expr->right );
			its_stack.push_back( expr->left  );
		}
		else if ( const Symbol* sym = v.sym() )
		{
			its_stack.push_back( sym->get() );
		}
	}
	
	void full_iterator::prune()
	{
		ASSERT( ! finished() );
		
		its_stack.pop_back();
	}
	
	Symbol* next_symbol( full_iterator& it )
	{
		while ( ! it.finished() )
		{
			if ( Symbol* sym = it.get().sym() )
			{
				return sym;
			}
			
			it.step();
		}
		
		return NULL;
	}
	
}
