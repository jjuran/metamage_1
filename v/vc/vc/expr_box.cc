/*
	expr_box.cc
	-----------
*/

#include "vc/expr_box.hh"

// iota
#include "iota/swap.hh"

// vc
#include "vc/value.hh"


namespace vc
{
	
	expr_box::~expr_box()
	{
		if ( its_expr )
		{
			while ( Expr* next = its_expr->right.expr.get() )
			{
				if ( intrusive_ptr_ref_count( its_expr ) > 1 )
				{
					break;
				}
				
				const_cast< Value& >( its_expr->right ).expr.release();
				
				intrusive_ptr_release( its_expr );
				
				its_expr = next;
			}
			
			intrusive_ptr_release( its_expr );
		}
	}
	
	expr_box::expr_box( const expr_box& that )
	:
		its_expr( that.its_expr )
	{
		if ( its_expr )
		{
			intrusive_ptr_add_ref( its_expr );
		}
	}
	
	expr_box& expr_box::operator=( const expr_box& that )
	{
		if ( that.its_expr )
		{
			intrusive_ptr_add_ref( that.its_expr );
		}
		
		if ( its_expr )
		{
			intrusive_ptr_release( its_expr );
		}
		
		its_expr = that.its_expr;
		
		return *this;
	}
	
	void expr_box::swap( expr_box& that )
	{
		using iota::swap;
		
		swap( its_expr, that.its_expr );
	}
	
	expr_box::expr_box( const Value& left, op_type op, const Value& right )
	{
		its_expr = new Expr( left, op, right );
		
		intrusive_ptr_add_ref( its_expr );
	}
	
}
