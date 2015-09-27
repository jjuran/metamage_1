/*
	value.hh
	--------
*/

#ifndef VC_VALUE_HH
#define VC_VALUE_HH

// plus
#include "plus/integer.hh"


namespace vc
{
	
	enum value_type
	{
		Value_nothing,
		Value_dummy_operand,
		Value_undefined,
		Value_boolean,
		Value_number,
	};
	
	struct Value
	{
		value_type     type;
		plus::integer  number;
		
		Value( value_type type = value_type() ) : type( type )
		{
		}
		
		Value( bool b ) : type( Value_boolean ), number( b )
		{
		}
		
		Value( const plus::integer& i ) : type( Value_number ), number( i )
		{
		}
	};
	
	void swap( Value& a, Value& b );
	
}

#endif
