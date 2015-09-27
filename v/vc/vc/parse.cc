/*
	parse.cc
	--------
*/

#include "vc/parse.hh"

// Standard C++
#include <vector>

// debug
#include "debug/assert.hh"

// plus
#include "plus/decimal.hh"

// vc
#include "vc/dyad.hh"
#include "vc/error.hh"
#include "vc/eval.hh"
#include "vc/functions.hh"
#include "vc/named_ops.hh"
#include "vc/ops.hh"
#include "vc/precedence.hh"
#include "vc/symbol_table.hh"
#include "vc/token.hh"


namespace vc
{
	
	class Parser
	{
		private:
			typedef std::vector< dyad  > Stack;
			typedef std::vector< Stack > Metastack;
			
			Stack      stack;
			Metastack  metastack;
		
		private:
			bool expecting_value() const;
			
			bool has_higher_precedence_op_than( op_type new_op ) const;
			
			void fold_ops_and_add( op_type op );
			
			Value result_from_subexpression();
			
			void push();
			void pop();
			
			void receive_value( const Value& term );
			
			void receive_op( op_type op );
			
			void receive_token( const Token& token );
		
		public:
			Value parse_and_eval( const char* p );
	};
	
	bool Parser::expecting_value() const
	{
		return stack.empty()  ||  stack.back().op != Op_none;
	}
	
	bool Parser::has_higher_precedence_op_than( op_type new_op ) const
	{
		if ( stack.size() < 2 )
		{
			return false;
		}
		
		/*
			 _______________________________
			| right operand | (no operator) |
			| left  operand | prev operator | <-- compare this to new_op
			|              ...              |
		*/
		
		return decreasing_op_precedence( stack.end()[ -2 ].op, new_op );
	}
	
	void Parser::fold_ops_and_add( op_type op )
	{
		Value& prev = stack.back().v;
		
		if ( is_symbol( prev.type )  &&  ! is_left_varop( op ) )
		{
			symbol_id sym = symbol_id( prev.number.clipped() );
			
			prev = lookup_symbol( sym );
		}
		
		while ( has_higher_precedence_op_than( op ) )
		{
			dyad right = stack.back();  stack.pop_back();
			dyad& left = stack.back();
			
			left.v  = eval( left.v, left.op, right.v );
		}
		
		stack.back().op = op;
	}
	
	Value Parser::result_from_subexpression()
	{
		fold_ops_and_add( Op_end );
		
		ASSERT( stack.size() == 1 );
		
		return stack.front().v;
	}
	
	void Parser::push()
	{
		if ( ! expecting_value() )
		{
			SYNTAX_ERROR( "left parenthesis where operator expected" );
		}
		
		metastack.push_back( stack );
		stack.clear();
	}
	
	void Parser::pop()
	{
		if ( expecting_value() )
		{
			SYNTAX_ERROR( "right parenthesis where value expected" );
		}
		
		if ( metastack.empty() )
		{
			SYNTAX_ERROR( "unbalanced right parenthesis" );
		}
		
		Value result = result_from_subexpression();
		
		stack = metastack.back();
		metastack.pop_back();
		
		stack.push_back( result );
	}
	
	void Parser::receive_value( const Value& x )
	{
		if ( ! expecting_value() )
		{
			SYNTAX_ERROR( "value where operator expected" );
		}
		
		stack.push_back( dyad( x ) );
	}
	
	void Parser::receive_op( op_type op )
	{
		if ( op == Op_none )
		{
			SYNTAX_ERROR( "operator used out of context" );
		}
		
		if ( expecting_value() )
		{
			// left unary operator
			
			stack.push_back( dyad( op ) );  // dummy left operand
		}
		else
		{
			// infix binary operator
			
			fold_ops_and_add( op );
		}
	}
	
	void Parser::receive_token( const Token& token )
	{
		switch ( token )
		{
			case Token_invalid:
				SYNTAX_ERROR( "invalid token" );
			
			case Token_whitespace:
				break;
			
			case Token_lparen:
				push();
				break;
			
			case Token_rparen:
				pop();
				break;
			
			case Token_digits:
				receive_value( decode_decimal( token.text ) );
				break;
			
			case Token_bareword:
				op_type op;
				
				if ( expecting_value() )
				{
					if ( function_id f = function_from_name( token.text ) )
					{
						receive_value( plus::integer( f ) );
						
						op = Op_function;
					}
					else if ( symbol_id sym = locate_symbol( token.text ) )
					{
						receive_value( sym );
						break;
					}
					else
					{
						SYNTAX_ERROR( "undeclared symbol" );
					}
				}
				else
				{
					op = op_from_name( token.text );
					
					if ( op == Op_none )
					{
						SYNTAX_ERROR( "invalid named operator" );
					}
				}
				
				receive_op( op );
				break;
			
			default:
				receive_op( op_from_token( token, expecting_value() ) );
				break;
		}
	}
	
	Value Parser::parse_and_eval( const char* p )
	{
		while ( true )
		{
			Token token;
			
			while (( token = next_token( p ) ))
			{
				if ( token == Token_semicolon )
				{
					break;
				}
				
				receive_token( token );
			}
			
			if ( ! metastack.empty() )
			{
				SYNTAX_ERROR( "premature end of parenthesized expression" );
			}
			
			Value result;  // nothing
			
			if ( ! stack.empty() )
			{
				if ( expecting_value() )
				{
					SYNTAX_ERROR( "premature end of expression" );
				}
				
				result = result_from_subexpression();
				
				stack.pop_back();
			}
			
			if ( ! token )
			{
				return result;
			}
		}
	}
	
	Value parse_and_eval( const char* p )
	{
		Parser parser;
		
		return parser.parse_and_eval( p );
	}
	
}
