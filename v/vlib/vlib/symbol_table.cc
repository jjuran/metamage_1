/*
	symbol_table.cc
	---------------
*/

#include "vlib/symbol_table.hh"

// Standard C++
#include <vector>

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
			
			void assign( const Value& v )  { its_value = v; }
			
			Value& deref()  { return its_value; }
			
			const Value& get() const  { return its_value; }
			
			bool is_const() const  { return its_type == Symbol_const; }
			bool is_var()   const  { return its_type != Symbol_const; }
			
			const plus::string& name() const  { return its_name; }
			
			bool is_defined() const  { return ! is_undefined( its_value ); }
	};
	
	static std::vector< Symbol > symbol_table;
	
	
	symbol_id locate_symbol( const plus::string& name )
	{
		typedef std::vector< Symbol >::const_iterator Iter;
		
		Iter begin = symbol_table.begin();
		Iter it    = symbol_table.end();
		
		while ( --it > begin )
		{
			if ( name == it->name() )
			{
				return symbol_id( it - begin );
			}
		}
		
		return symbol_id_none;
	}
	
	symbol_id create_symbol( const plus::string& name, symbol_type type )
	{
		if ( symbol_id sym = locate_symbol( name ) )
		{
			Symbol& var = symbol_table[ sym ];
			
			if ( type == Symbol_const  &&  var.is_var() )
			{
				var.constify();
				
				return sym;
			}
			
			return symbol_id_none;
		}
		
		symbol_id result = symbol_id( symbol_table.size() );
		
		symbol_table.push_back( Symbol( type, name ) );
		
		return result;
	}
	
	void assign_symbol( symbol_id id, const Value& value )
	{
		Symbol& var = symbol_table[ id ];
		
		if ( var.is_const()  &&  var.is_defined() )
		{
			SYMBOL_ERROR( "reassignment of constant" );
		}
		
		var.assign( value );
	}
	
	const Value& lookup_symbol( symbol_id id )
	{
		return symbol_table[ id ].get();
	}
	
	Value& modify_symbol( symbol_id id )
	{
		if ( symbol_table[ id ].is_const() )
		{
			SYMBOL_ERROR( "modification of constant" );
		}
		
		return symbol_table[ id ].deref();
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
