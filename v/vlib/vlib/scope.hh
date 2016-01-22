/*
	scope.hh
	--------
*/

#ifndef VLIB_SCOPE_HH
#define VLIB_SCOPE_HH

// vlib
#include "vlib/symbol_table.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	class lexical_scope
	{
		private:
			lexical_scope* its_parent;
			
			symbol_table  its_symbols;
			Symbols       its_symbol_cache;
		
		private:
			// non-copyable
			lexical_scope           ( const lexical_scope& );
			lexical_scope& operator=( const lexical_scope& );
		
		public:
			lexical_scope( lexical_scope* parent = NULL ) : its_parent( parent )
			{
				declare( "_", Symbol_var );
			}
			
			const Value& resolve( const plus::string& name, int depth = 0 );
			const Value& declare( const plus::string& name, symbol_type type );
			
			Value symbols() const  { return its_symbols.list(); }
			
			lexical_scope* parent() const  { return its_parent; }
	};
	
	class lexical_scope_box
	{
		private:
			lexical_scope* its_bottom_scope;
			lexical_scope* its_lexical_scope;
		
		private:
			// non-copyable
			lexical_scope_box           ( const lexical_scope_box& );
			lexical_scope_box& operator=( const lexical_scope_box& );
		
		public:
			lexical_scope_box( lexical_scope* parent = NULL )
			:
				its_bottom_scope( parent ),
				its_lexical_scope( new lexical_scope( parent ) )
			{
			}
			
			~lexical_scope_box()
			{
				lexical_scope* scope = its_lexical_scope;
				
				do
				{
					lexical_scope* parent = scope->parent();
					
					delete scope;
					
					scope = parent;
				}
				while ( scope != its_bottom_scope );
			}
			
			void push()
			{
				its_lexical_scope = new lexical_scope( its_lexical_scope );
			}
			
			void pop();
			
			lexical_scope* get() const  { return its_lexical_scope; }
			
			lexical_scope* operator->() const  { return get(); }
			lexical_scope& operator*() const  { return *get(); }
	};
	
}

#endif
