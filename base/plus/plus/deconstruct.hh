/*
	deconstruct.hh
	--------------
*/

#ifndef PLUS_DECONSTRUCT_HH
#define PLUS_DECONSTRUCT_HH


namespace plus
{
	
	class var_string;
	
	
	template < class Freeze, class Stringify >
	struct deconstruct
	{
		typedef Freeze     freeze;
		typedef Stringify  stringify;
		
		template < class Type >
		static void apply( var_string& out, const Type& data, bool binary )
		{
			if ( binary )
			{
				freeze::apply( out, data );
			}
			else
			{
				stringify::apply( out, data );
			}
		}
	};
	
}

#endif

