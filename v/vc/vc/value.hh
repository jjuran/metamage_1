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
		Value_none,
		Value_number,
	};
	
	struct Value
	{
		value_type     type;
		plus::integer  number;
		
		Value() : type()
		{
		}
		
		Value( const plus::integer& i ) : type( Value_number ), number( i )
		{
		}
	};
	
	void swap( Value& a, Value& b );
	
}

#endif
