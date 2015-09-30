/*
	value.hh
	--------
*/

#ifndef VC_VALUE_HH
#define VC_VALUE_HH

// plus
#include "plus/integer.hh"
#include "plus/string.hh"

// vc
#include "vc/symbol_id.hh"


namespace vc
{
	
	enum value_type
	{
		Value_nothing,
		Value_dummy_operand,
		Value_undefined,
		Value_empty_list,
		Value_symbol_declarator,
		Value_symbol,
		Value_boolean,
		Value_number,
		Value_string,
	};
	
	inline
	bool is_symbol( value_type type )
	{
		return type == Value_symbol  ||  type == Value_symbol_declarator;
	}
	
	struct Value
	{
		value_type     type;
		plus::integer  number;
		plus::string   string;
		
		Value( value_type type = value_type() ) : type( type )
		{
		}
		
		Value( symbol_id sym ) : type( Value_symbol ), number( sym )
		{
		}
		
		Value( bool b ) : type( Value_boolean ), number( b )
		{
		}
		
		Value( const plus::integer& i ) : type( Value_number ), number( i )
		{
		}
		
		Value( const plus::string& s ) : type( Value_string ), string( s )
		{
		}
		
		Value( const char* s ) : type( Value_string ), string( s )
		{
		}
	};
	
	void swap( Value& a, Value& b );
	
}

#endif
