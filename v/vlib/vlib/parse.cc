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
#include "vlib/proc_info.hh"
#include "vlib/quote.hh"
#include "vlib/symbol_table.hh"
#include "vlib/token.hh"
#include "vlib/types.hh"


namespace vlib
{
	
	static
	Value expression( const Value&  left,
	                  op_type       op,
	                  const Value&  right )
	{
		if ( op == Op_denote )
		{
			return eval( left, op, right );
		}
		
		return Value( Value_pair, left, op, right );
	}
	
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
			Value parse( const char* p );
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
			
			left.v  = expression( left.v, left.op, right.v );
		}
		
		stack.back().op = op;
	}
	
	void Parser::push( op_type op )
	{
		if ( op == Op_parens  &&  ! expecting_value() )
		{
			// Assume a function call.
			op_type op = Op_function;
			
			if ( stack.back().v.type() == Value_symbol_declarator )
			{
				// Nope, it's a type annotation.
				op = Op_denote;
			}
			
			fold_ops_and_add( op );
		}
		
		stack.push_back( op );
	}
	
	static
	bool ends_in_empty_statement( const std::vector< dyad >& stack )
	{
		if ( stack.empty() )
		{
			return false;  // Only happens if '}' wasn't balanced.
		}
		
		return stack.back().op == Op_end  ||  stack.back().op == Op_braces;
	}
	
	static
	bool is_empty_array( const std::vector< dyad >& stack )
	{
		if ( stack.empty() )
		{
			return false;  // Only happens if ']' wasn't balanced.
		}
		
		return stack.back().op == Op_brackets;
	}
	
	void Parser::pop( op_type op )
	{
		if ( expecting_value() )
		{
			if ( op == Op_braces  &&  ends_in_empty_statement( stack ) )
			{
				receive_value( Value_nothing );
			}
			else if ( op == Op_brackets  &&  is_empty_array( stack ) )
			{
				receive_value( Value_empty_list );
			}
			else
			{
				SYNTAX_ERROR( "right delimiter where value expected" );
			}
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
			
			if ( last.type() == V_str  &&  x.type() == V_str )
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
			
			if ( is_right_unary( op ) )
			{
				stack.push_back( dyad( Op_none ) );  // dummy right operand
			}
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
					stack.push_back( Op_array );
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
			
			case Token_lbrace:
				if ( expecting_value() )
				{
					stack.push_back( Op_block );
				}
				else
				{
					SYNTAX_ERROR( "left brace where operator expected" );
				}
				
				push( Op_braces );
				break;
			
			case Token_rbrace:
				pop( Op_braces );
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
					else if ( token.text == "if" )
					{
						op = Op_if;
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
	
	static
	op_type last_open( const std::vector< dyad >& stack )
	{
		typedef std::vector< dyad >::const_iterator Iter;
		
		Iter begin = stack.begin();
		Iter it    = stack.end();
		
		while ( --it >= begin )
		{
			switch ( it->op )
			{
				case Op_parens:
				case Op_brackets:
				case Op_braces:
					return it->op;
				
				default:
					break;
			}
		}
		
		return Op_none;
	}
	
	Value Parser::parse( const char* p )
	{
		Value result;  // nothing
		
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
			
			Value value;  // nothing
			
			if ( ! stack.empty() )
			{
				if ( expecting_value() )
				{
					if ( ! ends_in_empty_statement( stack ) )
					{
						SYNTAX_ERROR( "premature end of expression" );
					}
					
					receive_value( Value_nothing );
				}
				
				fold_ops_and_add( Op_end );
				
				if ( stack.size() == 1 )
				{
					value = stack.back().v;
					
					stack.pop_back();
				}
				else if ( last_open( stack ) == Op_braces )
				{
					continue;
				}
				else
				{
					SYNTAX_ERROR( "premature end of delimited group" );
				}
			}
			
			if ( ! result.type() )
			{
				result = value;
			}
			else
			{
				result = Value( Value_pair, result, Op_end, value );
			}
			
			if ( ! token )
			{
				return eval( result );
			}
		}
	}
	
	static proc_info proc_invoke = { &eval_tree, "invoke", 0 };
	
	static const Value invoke = proc_invoke;
	
	Value eval_tree( const Value& tree )
	{
		if ( Expr* expr = tree.expr() )
		{
			if ( expr->op == Op_end )
			{
				return eval_tree( expr->left ), eval_tree( expr->right );
			}
			
			if ( expr->op == Op_block )
			{
				return Value( Value_pair, invoke, Op_block, expr->right );
			}
			
			const Value* left  = &expr->left;
			const Value* right = &expr->right;
			
			if ( left->type() == Value_dummy_operand )
			{
				using iota::swap;
				
				swap( left, right );
			}
			
			if ( is_left_varop( expr->op ) )
			{
				return eval( *left, expr->op, eval_tree( *right ) );
			}
			
			/*
				WARNING:  This recursion is unsafe.  It can be abused with a
				sufficiently long program, and recursive user-defined blocks
				only make this easier.
			*/
			
			return eval( eval_tree( *left ),
			             expr->op,
			             eval_tree( *right ) );
		}
		
		return eval( tree );
	}
	
	Value parse( const char* p )
	{
		static bool installed = install_basic_symbols();
		
		Parser parser;
		
		return parser.parse( p );
	}
	
}
