/*
	new_line.cc
	-----------
*/

#include "vlib/new_line.hh"

// vlib
#include "vlib/named_ops.hh"
#include "vlib/token.hh"


namespace vlib
{
	
	/*
		Barewords:
		
			if ... then
			{ ... }
			else
			
			Binary operators are continuations.
			
			do
			{ ... }
			while ...
			
			Ambiguous operators are also continuations.
			
			var x = 3
			var y = 4
			
			Prefix-only operators are new statements.
		
		Double newline:
		
			foo
			
			bar
			
			Assume separate statements.
		
		Increment/decrement:
		
			foo
			++ bar
			
			Assume prefix in a new statement, not postfix.
		
		Right brace:
		
			++x
			}
			
			Don't leak the value of x to callers.  (Use `return` for that.)
		
		Left bracket:
		
			foo
			[bar]
			
			Reserve this syntax for directives.
		
		Left parenthesis:
		
			foo
			(bar)
			
			Assume a new statement, not function arguments.
	*/
	
	bool new_line_continues( const Token& token )
	{
		switch ( token )
		{
			// Continue if a name is a binary operator; break otherwise.
			case Token_bareword:
			case Token_bareword_function:
				return op_from_name( token.text );
			
			case Token_newline:
			case Token_plus_x2:
			case Token_minus_x2:
			case Token_rbrace:
			case Token_lbracket:
			case Token_lparen:
				return false;
			
			default:
				break;
		}
		
		return true;
	}
	
}
