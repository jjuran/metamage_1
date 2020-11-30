/*
	expression.cc
	-------------
*/

#include "expression.hh"

// mxcpp
#include "atom.hh"
#include "binary_ops.hh"
#include "exception.hh"
#include "expand.hh"
#include "macro.hh"
#include "tokenize.hh"
#include "value.hh"


namespace tool
{
	
	static value_t eval( const token_list& tokens, std::size_t& i );
	
	
	static value_t eval_term( const token_list& tokens, std::size_t& i )
	{
		
	tail_call:
		
		const plus::string& token = tokens.get()[i];
		
		const char* p = token.c_str();
		
		switch ( p[0] )
		{
			case '*':
			case '&':
				throw exception( "bad_preprocessor_op" );
			
			case '.':
			case '"':
				throw exception( "bad_preprocessor_constant" );
			
			case '+':
			case '-':
				if ( p[1] == p[0]  ||  p[1] == '=' )
				{
					throw exception( "bad_preprocessor_op" );
				}
			
			default:
				break;
		}
		
		++i;
		
		switch ( *p )
		{
			case '+':
				goto tail_call;
			
			case '-':
				return -eval_term( tokens, i );
			
			case '!':
				return !eval_term( tokens, i );
			
			case '~':
				return ~eval_term( tokens, i );
			
			case '(':
				return eval( tokens, i );
			
			default:
				break;
		}
		
		return eval_atom( p );
	}
	
	
	static value_t eval_op( const token_list& tokens, std::size_t& i, const value_t& a )
	{
		const binary_operation* op = next_binary_operator( tokens.get()[i].c_str() );
		
		if ( op == NULL )
		{
			return a;
		}
		
		++i;
		
		value_t b = eval_term( tokens, i );
		
		if ( i < tokens.get().size() )
		{
			const binary_operation* next_op = next_binary_operator( tokens.get()[i].c_str() );
			
			if ( next_op  &&  next_op->rank + is_right_associative( op ) > op->rank )
			{
				// Higher precedence, or equal and right-associative
				b = eval_op( tokens, i, b );
			}
		}
		
		return op->code( a, b );
	}
	
	static value_t eval( const token_list& tokens, std::size_t& i )
	{
		value_t operand = eval_term( tokens, i );
		
		while ( i < tokens.get().size() )
		{
			if ( tokens.get()[i] == ")" )
			{
				++i;
				
				break;
			}
			
			operand = eval_op( tokens, i, operand );
		}
		
		return operand;
	}
	
	bool expression_is_true( const plus::string& expression )
	{
		const bool in_expression_or_not = true;
		
		token_list tokens;
		
		// #if expressions are always a single (logical) line
		(void) expand_macros( tokenize( expression ), tokens, in_expression_or_not );
		
		std::size_t i = 0;
		
		value_t result = eval( tokens, i );
		
		return result;
	}
	
}
