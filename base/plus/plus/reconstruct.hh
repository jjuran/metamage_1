/*
	reconstruct.hh
	--------------
*/

#ifndef PLUS_RECONSTRUCT_HH
#define PLUS_RECONSTRUCT_HH


namespace plus
{
	
	template < class Type, class Thaw, class Vivify >
	struct reconstruct
	{
		typedef Type result_type;
		
		typedef Thaw    thaw;
		typedef Vivify  vivify;
		
		static Type apply( const char* begin, const char* end, bool binary )
		{
			return binary ? thaw  ::apply( begin, end )
			              : vivify::apply( begin, end );
		}
	};
	
}

#endif

