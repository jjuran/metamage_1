/*
	expr_box.hh
	-----------
*/

#ifndef VLIB_EXPRBOX_HH
#define VLIB_EXPRBOX_HH

// vlib
#include "vlib/op_type.hh"


namespace vlib
{
	
	struct Expr;
	struct Value;
	
	class expr_box
	{
		private:
			Expr* its_expr;
		
		public:
			expr_box() : its_expr()
			{
			}
			
			~expr_box();
			
			expr_box           ( const expr_box& );
			expr_box& operator=( const expr_box& );
			
			void swap( expr_box& that );
			
			expr_box( const Value& left, op_type op, const Value& right );
			
			Expr* get() const  { return its_expr; }
			
			Expr* release()
			{
				Expr* result = get();
				its_expr = 0;
				return result;
			}
	};
	
	inline
	void swap( expr_box& a, expr_box& b )
	{
		a.swap( b );
	}
	
}

#endif
