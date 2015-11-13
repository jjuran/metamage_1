/*
	symbol_table.cc
	---------------
*/

#include "vlib/symbol_table.hh"

// Standard C++
#include <list>

// vlib
#include "vlib/error.hh"


namespace vlib
{
	
	class Symbol
	{
		private:
			plus::string  its_name;
			Value         its_value;
			symbol_type   its_type;
		
		public:
			Symbol() : its_type()
			{
			}
			
			Symbol( symbol_type          type,
					const plus::string&  name,
					const Value&         value = Value_undefined )
			:
				its_name( name ),
				its_value( value ),
				its_type( type )
			{
			}
			
			void constify()  { its_type = Symbol_const; }
			
			void assign( const Value& v );
			
			Value& deref();
			
			const Value& get() const  { return its_value; }
			
			bool is_const() const  { return its_type == Symbol_const; }
			bool is_var()   const  { return its_type != Symbol_const; }
			
			const plus::string& name() const  { return its_name; }
			
			bool is_defined() const  { return ! is_undefined( its_value ); }
	};
	
	void Symbol::assign( const Value& v )
	{
		if ( is_const()  &&  is_defined() )
		{
			SYMBOL_ERROR( "reassignment of constant" );
		}
		
		its_value = v;
	}
	
	Value& Symbol::deref()
	{
		if ( is_const() )
		{
			SYMBOL_ERROR( "modification of constant" );
		}
		
		return its_value;
	}
	
	static std::list< Symbol > symbol_table;
	
	
	symbol_id locate_symbol( const plus::string& name )
	{
		typedef std::list< Symbol >::iterator Iter;
		
		Iter begin = symbol_table.begin();
		Iter it    = symbol_table.end();
		
		while ( --it != begin )
		{
			if ( name == it->name() )
			{
				return &*it;
			}
		}
		
		return NULL;
	}
	
	symbol_id create_symbol( const plus::string& name, symbol_type type )
	{
		if ( symbol_id sym = locate_symbol( name ) )
		{
			Symbol& var = *sym;
			
			if ( type == Symbol_const  &&  var.is_var() )
			{
				var.constify();
				
				return sym;
			}
			
			return NULL;
		}
		
		symbol_table.push_back( Symbol( type, name ) );
		
		return &symbol_table.back();
	}
	
	void assign_symbol( symbol_id id, const Value& value )
	{
		id->assign( value );
	}
	
	const Value& lookup_symbol( symbol_id id )
	{
		return id->get();
	}
	
	Value& modify_symbol( symbol_id id )
	{
		return id->deref();
	}
	
	
	static inline
	Symbol constant( const char* name, const Value& value )
	{
		return Symbol( Symbol_const, name, value );
	}
	
	struct symbol_table_init
	{
		symbol_table_init();
	};
	
	symbol_table_init::symbol_table_init()
	{
		symbol_table.push_back( constant( "", Value_undefined ) );
		
		symbol_table.push_back( constant( "false", Value( false ) ) );
		symbol_table.push_back( constant( "true",  Value( true  ) )  );
	}
	
	static symbol_table_init init;
	
}
