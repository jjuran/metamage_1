/*
	parse.cc
	--------
*/

#include "vlib/parse.hh"

// Standard C++
#include <vector>

// debug
#include "debug/assert.hh"

// plus
#include "plus/decimal.hh"
#include "plus/decode_binoid_int.hh"
#include "plus/string/concat.hh"

// vlib
#include "vlib/dyad.hh"
#include "vlib/error.hh"
#include "vlib/eval.hh"
#include "vlib/functions.hh"
#include "vlib/init.hh"
#include "vlib/named_ops.hh"
#include "vlib/ops.hh"
#include "vlib/precedence.hh"
#include "vlib/quote.hh"
#include "vlib/symbol_table.hh"
#include "vlib/token.hh"
#include "vlib/types.hh"


namespace vlib
{
	
	
	class Parser
	{
		private:
			typedef std::vector< dyad > Stack;
			
			Stack stack;
		
		private:
			bool expecting_value() const;
			
			bool has_higher_precedence_op_than( op_type new_op ) const;
			
			void fold_ops_and_add( op_type op );
			
			void push( op_type op );
			void pop( op_type op );
			
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
		while ( has_higher_precedence_op_than( op ) )
		{
			dyad right = stack.back();  stack.pop_back();
			dyad& left = stack.back();
			
			left.v  = eval( left.v, left.op, right.v );
		}
		
		stack.back().op = op;
	}
	
	void Parser::push( op_type op )
	{
		if ( op == Op_parens  &&  ! expecting_value() )
		{
			// Assume a function call.
			op_type op = Op_function;
			
			if ( get_type( stack.back().v ) == Value_symbol_declarator )
			{
				// Nope, it's a type annotation.
				op = Op_denote;
			}
			
			fold_ops_and_add( op );
		}
		
		stack.push_back( op );
	}
	
	void Parser::pop( op_type op )
	{
		if ( expecting_value() )
		{
			SYNTAX_ERROR( "right delimiter where value expected" );
		}
		
		fold_ops_and_add( Op_end );
		
		if ( stack.size() < 2  ||  stack.end()[ -2 ].op != op )
		{
			SYNTAX_ERROR( "unbalanced right delimiter" );
		}
		
		// Remove the sentinel and clear Op_end.
		
		stack.end()[ -2 ].v  = stack.end()[ -1 ].v;
		stack.end()[ -2 ].op = Op_none;
		
		stack.pop_back();
	}
	
	void Parser::receive_value( const Value& x )
	{
		if ( ! expecting_value() )
		{
			Value& last = stack.back().v;
			
			// Try string concatenation.
			
			if ( get_type( last ) == V_str  &&  get_type( x ) == V_str )
			{
				last = get_str( last ) + get_str( x );
				
				return;
			}
			
			// Assume a function call.
			
			fold_ops_and_add( is_empty( x ) ? Op_function : Op_named_unary );
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
			
			case Token_lbracket:
				if ( expecting_value() )
				{
					SYNTAX_ERROR( "array literals unimplemented" );
				}
				else
				{
					fold_ops_and_add( Op_subscript );
				}
				
				push( Op_brackets );
				break;
			
			case Token_rbracket:
				pop( Op_brackets );
				break;
			
			case Token_lparen:
				push( Op_parens );
				break;
			
			case Token_rparen:
				pop( Op_parens );
				break;
			
			case Token_parens:
				receive_value( empty_list );
				break;
			
			case Token_dot_x3:
				receive_value( etc_vtype );
				break;
			
			case Token_bin:
				receive_value( unbin_int( token.text.substr( 2 ) ) );
				break;
			
			case Token_hex:
				receive_value( unhex_int( token.text.substr( 2 ) ) );
				break;
			
			case Token_digits:
				receive_value( decode_decimal( token.text ) );
				break;
			
			case Token_unbin:
				receive_value( proc_unbin );
				fold_ops_and_add( Op_function );
				break;
			
			case Token_unhex:
				receive_value( proc_unhex );
				fold_ops_and_add( Op_function );
				break;
			
			case Token_byte:
				receive_value( Value::byte( unquote_byte( token.text ) ) );
				break;
			
			case Token_string:
				receive_value( unquote_string( token.text ) );
				break;
			
			case Token_string_escaped:
				receive_value( unquote_escaped_string( token.text ) );
				break;
			
			case Token_bareword:
			case Token_bareword_function:
				op_type op;
				
				if ( expecting_value() )
				{
					op = Op_none;
				}
				else
				{
					op = op_from_name( token.text );
				}
				
				if ( op == Op_none )
				{
					if ( token.text == "const" )
					{
						op = Op_const;
					}
					else if ( token.text == "var" )
					{
						op = Op_var;
					}
					else
					{
						symbol_id sym;
						
						if ( ! stack.empty() )
						{
							if ( stack.back().op == Op_member )
							{
								receive_value( token.text );
								break;
							}
							
							if ( declares_symbols( stack.back().op ) )
							{
								bool is_var = stack.back().op - Op_const;
								symbol_type type = symbol_type( is_var );
								
								sym = create_symbol( token.text, type );
								
								stack.pop_back();
								
								receive_value( Value( sym, int() ) );
								break;
							}
						}
						
						if ( symbol_id sym = locate_symbol( token.text ) )
						{
							receive_value( sym );
							break;
						}
						
						SYMBOL_ERROR( "undeclared symbol" );
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
			
			Value result;  // nothing
			
			if ( ! stack.empty() )
			{
				if ( expecting_value() )
				{
					SYNTAX_ERROR( "premature end of expression" );
				}
				
				fold_ops_and_add( Op_end );
				
				result = stack.back().v;
				
				stack.pop_back();
				
				if ( ! stack.empty() )
				{
					SYNTAX_ERROR( "premature end of delimited group" );
				}
			}
			
			if ( ! token )
			{
				return eval( result );
			}
		}
	}
	
	Value parse_and_eval( const char* p )
	{
		static bool installed = install_basic_symbols();
		
		Parser parser;
		
		return parser.parse_and_eval( p );
	}
	
}
