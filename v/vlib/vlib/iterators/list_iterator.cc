/*
	list_iterator.cc
	----------------
*/

#include "vlib/iterators/list_iterator.hh"


namespace vlib
{
	
	list_iterator::list_iterator( const Value& list )
	{
		its_list = list;
		its_mark = &its_list;
	}
	
	const Value& list_iterator::get() const
	{
		if ( Expr* expr = its_mark->listexpr() )
		{
			return expr->left;
		}
		
		return *its_mark;
	}
	
	void list_iterator::step()
	{
		if ( Expr* expr = its_mark->listexpr() )
		{
			its_mark = &expr->right;
		}
		else
		{
			its_mark = &empty_list;
		}
	}
	
}
