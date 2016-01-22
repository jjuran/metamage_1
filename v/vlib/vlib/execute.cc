/*
	execute.cc
	----------
*/

#include "vlib/execute.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/eval.hh"
#include "vlib/proc_info.hh"
#include "vlib/symbol.hh"
#include "vlib/symdesc.hh"
#include "vlib/tracker.hh"


namespace vlib
{
	
	static
	Value gensym( symbol_type type, const plus::string& name, const Value &v )
	{
		const Value symbol( Symbol_var, name );
		
		track_symbol( symbol, v );
		
		symbol.sym()->assign( v );
		
		return symbol;
	}
	
	static
	Value unshare_symbols( const Value& presets )
	{
		if ( is_empty( presets ) )
		{
			return Value_empty_list;
		}
		
		const Value& head = first( presets );
		const Value& tail = rest ( presets );
		
		const Symbol* sym = head.sym();
		
		const Value new_head = sym->is_immutable() ? head : sym->clone();
		
		if ( is_empty( tail ) )
		{
			return new_head;
		}
		
		return Value( new_head, unshare_symbols( tail ) );
	}
	
	static
	Value execute( const Value& tree, const Value& stack );
	
	static
	Value invoke_block( const Value& block, const Value& arguments )
	{
		Expr* expr = block.expr();
		
		ASSERT( expr );
		
		ASSERT( expr->op == Op_activation );
		
		const Value& stack = expr->left;
		const Value& entry = expr->right;
		
		expr = stack.expr();
		
		ASSERT( expr );
		
		ASSERT( expr->op == Op_frame );
		
		const Value& caller = expr->left;
		const Value& locals = expr->right;
		
		const bool is_call = arguments.type();
		
		const Value underscore = is_call ? gensym( Symbol_var, "_", arguments )
		                                 : first( locals );
		
		const Value new_frame( underscore, unshare_symbols( rest( locals ) ) );
		const Value new_stack( caller, Op_frame, new_frame );
		
		return execute( entry, new_stack );
	}
	
	static
	Value v_invoke( const Value& v )
	{
		Expr* expr = v.expr();
		
		ASSERT( expr != NULL );
		
		if ( expr->op == Op_bind_args )
		{
			const Value& block = expr->left;
			const Value& args  = expr->right;
			
			return invoke_block( block, args );
		}
		
		if ( expr->op == Op_activation )
		{
			return invoke_block( v, nothing );
		}
		
		ASSERT( expr->op == Op_expression );
		
		return execute( expr->right, expr->left );
	}
	
	static proc_info proc_invoke = { &v_invoke, "invoke", 0 };
	
	static
	Value resolve_symbol_list( const Value& v, const Value& stack )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_denote )
			{
				const Value& left = resolve_symbol( expr->left, stack );
				
				return eval( left, Op_denote, expr->right );
			}
			
			if ( expr->op == Op_list )
			{
				return Value( resolve_symbol_list( expr->left,  stack ),
				              resolve_symbol_list( expr->right, stack ) );
			}
		}
		
		return resolve_symbol( v, stack );
	}
	
	Value execute( const Value& tree, const Value& stack )
	{
		if ( Expr* expr = tree.expr() )
		{
			if ( expr->op == Op_var  ||  expr->op == Op_const )
			{
				return Value_nothing;
			}
			
			if ( expr->op == Op_end )
			{
				execute( expr->left, stack );
				
				return execute( expr->right, stack );
			}
			
			if ( expr->op == Op_block )
			{
				const Value& scope = expr->right;
				
				expr = scope.expr();
				
				ASSERT( expr );
				
				ASSERT( expr->op == Op_scope );
				
				const Value& data = expr->left;   // args and local symbols
				const Value& code = expr->right;  // expression tree
				
				const Value& underscore = first( stack.expr()->right );
				
				const Value new_frame( underscore, rest( data ) );
				
				const Value new_stack( stack, Op_frame, new_frame );
				const Value activation( new_stack, Op_activation, code );
				
				return Value( proc_invoke, Op_invocation, activation );
			}
			
			if ( expr->op == Op_do )
			{
				const Value expression( stack, Op_expression, expr->left );
				const Value test( proc_invoke, Op_invocation, expression );
				
				return Value( test, Op_do, execute( expr->right, stack ) );
			}
			
			const Value* left  = &expr->left;
			const Value* right = &expr->right;
			
			if ( left->type() == Value_dummy_operand )
			{
				using iota::swap;
				
				swap( left, right );
			}
			
			if ( is_left_varop( expr->op )  &&  ! is_type_annotation( *left ) )
			{
				return eval( resolve_symbol_list( *left, stack ),
				             expr->op,
				             execute( *right, stack ) );
			}
			
			/*
				WARNING:  This recursion is unsafe.  It can be abused with a
				sufficiently long program, and recursive user-defined blocks
				only make this easier.
			*/
			
			return eval( execute( *left, stack ),
			             expr->op,
			             execute( *right, stack ) );
		}
		
		const Value& resolved = resolve_symbol( tree, stack );
		
		return eval( resolved );
	}
	
	Value execute( const Value& root )
	{
		Expr* expr = root.expr();
		
		const Value stack( Value_nothing, Op_frame, expr->left );
		
		return execute( expr->right, stack );
	}
	
}
