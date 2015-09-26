/*
	symbol_table.cc
	---------------
*/

#include "vc/symbol_table.hh"

// Standard C++
#include <vector>

// vc
#include "vc/error.hh"


namespace vc
{
	
	struct Symbol
	{
		plus::string  name;
		Value         value;
		symbol_type   type;
		
		Symbol() : type()
		{
		}
		
		Symbol( const plus::string& name, const Value& value, symbol_type type )
		:
			name( name ),
			value( value ),
			type( type )
		{
		}
	};
	
	static std::vector< Symbol > symbol_table;
	
	
	symbol_id locate_symbol( const plus::string& name )
	{
		typedef std::vector< Symbol >::const_iterator Iter;
		
		Iter begin = symbol_table.begin();
		Iter it    = symbol_table.end();
		
		while ( --it > begin )
		{
			if ( name == it->name )
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
			
			if ( type == Symbol_const  &&  var.type != Symbol_const )
			{
				var.type = Symbol_const;
				
				return sym;
			}
			
			return symbol_id_none;
		}
		
		symbol_id result = symbol_id( symbol_table.size() );
		
		symbol_table.push_back( Symbol( name, Value_undefined, type ) );
		
		return result;
	}
	
	void assign_symbol( symbol_id id, const Value& value )
	{
		Symbol& var = symbol_table[ id ];
		
		if ( var.type == Symbol_const  &&  var.value.type != Value_undefined )
		{
			SYMBOL_ERROR( "reassignment of constant" );
		}
		
		var.value = value;
	}
	
	const Value& lookup_symbol( symbol_id id )
	{
		return symbol_table[ id ].value;
	}
	
	
	static inline
	Symbol constant( const char* name, const Value& value )
	{
		return Symbol( name, value, Symbol_const );
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
