/*
	symbol_table.cc
	---------------
*/

#include "vc/symbol_table.hh"

// Standard C++
#include <vector>


namespace vc
{
	
	struct Symbol
	{
		plus::string  name;
		Value         value;
		
		Symbol()
		{
		}
		
		Symbol( const plus::string& name, const Value& value )
		:
			name( name ),
			value( value )
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
	
	const Value& lookup_symbol( symbol_id id )
	{
		return symbol_table[ id ].value;
	}
	
	
	static inline
	Symbol constant( const char* name, const Value& value )
	{
		return Symbol( name, value );
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
