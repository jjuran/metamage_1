/*
	analyze.cc
	----------
*/

#include "vlib/analyze.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/fold.hh"
#include "vlib/scope.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/bareword.hh"
#include "vlib/types/null.hh"
#include "vlib/types/string.hh"
#include "vlib/types/term.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	void insert_code_to_unpack_arguments( const Value& f, const Value& params )
	{
		if ( Expr* expr = f.expr() )
		{
			if ( expr->op == Op_block )
			{
				Value unpack = Value( params, Op_duplicate, Identifier( "_" ) );
				
				const Value& body = expr->right;
				
				Value rebuilt( unpack, Op_end, body );
				
				expr->right = rebuilt;
			}
		}
	}
	
	static
	void insert_code_to_unpack_into_v( const Value& f )
	{
		const Value const_v( Op_let, Identifier( "v" ) );
		
		insert_code_to_unpack_arguments( f, const_v );
	}
	
	static
	void insert_code_to_unpack_into_two( const Value&  f,
	                                     const char*   a,
	                                     const char*   b )
	{
		const Value const_a( Op_let, Identifier( a ) );
		const Value const_b( Op_let, Identifier( b ) );
		
		const Value ab_list( const_a, const_b );
		
		insert_code_to_unpack_arguments( f, ab_list );
	}
	
	static inline
	void insert_code_to_unpack_into_a_b( const Value& f )
	{
		insert_code_to_unpack_into_two( f, "a", "b" );
	}
	
	static inline
	void insert_code_to_unpack_into_L_v( const Value& f )
	{
		insert_code_to_unpack_into_two( f, "L", "v" );
	}
	
	static
	void insert_code_to_unpack_reducer( const Value& reducer )
	{
		if ( Expr* expr = reducer.expr() )
		{
			if ( expr->op == Op_block )
			{
				insert_code_to_unpack_into_a_b( reducer );
			}
			else if ( expr->op == Op_forward_init )
			{
				insert_code_to_unpack_into_L_v( expr->right );
			}
		}
	}
	
	static const Type one( one_vtype );
	
	static
	void insert_prototype_prelude( const Value& f, const Value& prototype )
	{
		list_iterator it( prototype );
		
		list_builder prelude;
		
		if ( ! it )
		{
			Value var( Op_var, Identifier( "" ) );
			
			prelude.append( Value( var, Op_denote, empty_list ) );
		}
		
		while ( it )
		{
			const Value& parameter = it.use();
			
			const Value* name = &parameter;
			const Value* type = &one;
			
			if ( Expr* expr = parameter.expr() )
			{
				if ( declares_symbols( expr->op ) )
				{
					prelude.append( parameter );
					continue;
				}
				
				if ( expr->op == Op_mapping )
				{
					if ( expr->left.type() == V_str )
					{
						// Destructively convert string to bareword
						expr->left.replace_dispatch_methods( &bareword_dispatch );
					}
					
					name = &expr->left;
					type = &expr->right;
					
					if ( name->expr()  &&  name->expr()->op == Op_named_param )
					{
						name = &name->expr()->left;
					}
				}
				else if ( expr->op == Op_named_param )
				{
					name = &expr->left;
				}
			}
			
			Value var( Op_var, *name );
			
			prelude.append( Value( var, Op_denote, *type ) );
		}
		
		insert_code_to_unpack_arguments( f, prelude.get() );
	}
	
	static
	void insert_prototype_prelude( Expr* expr )
	{
		ASSERT( expr->op == Op_function );
		
		const Value& L = expr->left;
		const Value& R = expr->right;
		
		if ( R.expr()  &&  R.expr()->op == Op_block )
		{
			Expr* L_expr = L.expr();
			
			if ( L_expr  &&  L_expr->op == Op_function )
			{
				const Value& LL = L_expr->left;
				
				Expr* def_expr = LL.expr();
				
				if ( def_expr  &&  def_expr->op == Op_export )
				{
					def_expr = def_expr->right.expr();
				}
				
				if ( def_expr  &&  def_expr->op == Op_def )
				{
					insert_prototype_prelude( R, L_expr->right );
					
					expr->right = Value( L_expr->right, Op_prototype, R );
					expr->left = LL;
				}
			}
		}
	}
	
	static
	void insert_lambda_prototype_prelude( Expr* expr )
	{
		ASSERT( expr->op == Op_lambda );
		
		const Value& R = expr->right;
		
		Expr* R_expr = R.expr();
		
		if ( R_expr->op != Op_function )
		{
			return;
		}
		
		const Value& RL = R_expr->left;
		const Value& RR = R_expr->right;
		
		if ( RR.expr()  &&  RR.expr()->op == Op_block )
		{
			insert_prototype_prelude( RR, RL );
			
			R_expr->op = Op_prototype;
		}
	}
	
	class Analyzer
	{
		private:
			lexical_scope_box its_scope;
			
			bool     it_is_a_module;
			unsigned its_export_count;
			
			Variable its_exports;
		
		private:
			Value enscope( const Value& block ) const;
			
			void visit( Value& syntree, const source_spec& source );
			
			void visit_prototype( Value& syntree );
		
		public:
			Analyzer( lexical_scope* globals )
			:
				its_scope( globals ),
				it_is_a_module(),
				its_export_count(),
				its_exports( "__export__" )
			{
			}
			
			Value analyze( Value syntree );
	};
	
	Value Analyzer::enscope( const Value& block ) const
	{
		return Value( its_scope->symbols(), Op_scope, block );
	}
	
	void Analyzer::visit_prototype( Value& v )
	{
		Value* list = &v;
		
		while ( Expr* expr = list->listexpr() )
		{
			Value& head = expr->left;
			
			if ( head.expr()  &&  head.expr()->op == Op_named_param )
			{
				visit( head.expr()->right, head.expr()->source );
			}
			
			list = &list->expr()->right;
		}
		
		/*
			Now, list points either to the last parameter,
			or to empty_list if there were no parameters.
		*/
		
		Value& last = *list;
		
		if ( last.expr()  &&  last.expr()->op == Op_named_param )
		{
			visit( last.expr()->right, last.expr()->source );
		}
	}
	
	void Analyzer::visit( Value& v, const source_spec& source )
	{
		if ( Expr* expr = v.expr() )
		{
			const op_type op = expr->op;
			
			if ( op == Op_module )
			{
				if ( it_is_a_module )
				{
					THROW( "duplicate `module` declaration" );
				}
				
				if ( its_export_count != 0 )
				{
					THROW( "`module` must precede `export`" );
				}
				
				it_is_a_module = true;
				
				Value exports( Op_export, empty_array );
				
				its_exports.sym()->deref() = exports;
				
				return;
			}
			else if ( op == Op_export )
			{
				if ( ! it_is_a_module  &&  its_export_count != 0 )
				{
					THROW( "only one export allowed in non-module" );
				}
				
				++its_export_count;
				
				expr->left = its_exports;
			}
			else if ( op == Op_block )
			{
				its_scope.push();
			}
			else if ( op == Op_map  ||  op == Op_ver  ||  op == Op_via )
			{
				insert_code_to_unpack_into_v( expr->right );
			}
			else if ( op == Op_gap )
			{
				insert_code_to_unpack_into_a_b( expr->right );
			}
			else if ( op == Op_per )
			{
				insert_code_to_unpack_reducer( expr->right );
			}
			else if ( op == Op_function )
			{
				insert_prototype_prelude( expr );
			}
			else if ( op == Op_lambda )
			{
				insert_lambda_prototype_prelude( expr );
			}
			else if ( op == Op_keyvar )
			{
				if ( const Identifier* name = expr->right.is< Identifier >() )
				{
					v = mapping( name->get(), *name );
					
					expr = v.expr();
				}
				else
				{
					THROW( "`:name` must be an identifier" );
				}
			}
			else if ( op == Op_present )
			{
				const Value* x = &expr->left;
				
				if ( const Identifier* identifier = x->is< Identifier >() )
				{
					if ( ! its_scope->resolve( x->string() ) )
					{
						x = &its_scope->declare( x->string(), Symbol_const );
						
						x->sym()->deref_unsafe() = Null();
					}
				}
				
				v = *x;
				
				visit( v, expr->source );
				return;
			}
			else if ( declares_symbols( op ) )
			{
				const Value* x = &expr->right;
				
				if ( op == Op_for )
				{
					Expr* e2 = x->expr();  // `do`
					ASSERT( e2 != NULL );
					
					e2 = e2->left.expr();  // `in`
					
					if ( e2 == NULL )
					{
						THROW( "`for` expects `in`" );
					}
					
					x = &e2->left;  // loop iteration variable
				}
				
				if ( ! x->is< Identifier >() )
				{
					THROW( "declared symbol must be an identifier" );
				}
				
				bool is_var = op == Op_var;
				symbol_type type = symbol_type( is_var );
				
				try
				{
					its_scope->declare( x->string(), type );
				}
				catch ( const exception& e )
				{
					throw user_exception( String( e.message ), expr->source );
				}
			}
			
			if ( op == Op_prototype )
			{
				visit_prototype( expr->left );
			}
			else if ( op != Op_named_param )
			{
				visit( expr->left, expr->source );
			}
			
			if ( op != Op_member  &&  op != Op_getter )
			{
				visit( expr->right, expr->source );
			}
			
			if ( op == Op_function )
			{
				if ( Expr* e2 = expr->left.expr() )
				{
					if ( declares_symbols( e2->op )  &&  e2->op != Op_def )
					{
						expr->op = Op_denote;
					}
				}
			}
			else if ( op == Op_block )
			{
				expr->right = enscope( expr->right );
				
				its_scope.pop();
			}
		}
		else if ( const Identifier* identifier = v.is< Identifier >() )
		{
			const plus::string& name = identifier->get();
			
			if ( const Value& sym = its_scope->resolve( name ) )
			{
				v = sym;
			}
			else
			{
				throw undeclared_symbol_error( name, source );
			}
		}
		
		const Value folded = fold( v, its_scope.get() );
		
		if ( folded )
		{
			v = folded;
		}
	}
	
	Value Analyzer::analyze( Value syntree )
	{
		try
		{
			visit( syntree, source_spec() );
			
			if ( its_export_count )
			{
				syntree = Value( syntree, Op_end, its_exports );
			}
			
			return enscope( syntree );
		}
		catch ( const exception& e )
		{
			throw user_exception( String( e.message ), source_spec() );
		}
		
		// Silence Metrowerks C++ warning
		return Value();
	}
	
	Value analyze( const Value& syntree, lexical_scope* globals )
	{
		Analyzer analyzer( globals );
		
		return analyzer.analyze( syntree );
	}
	
}
