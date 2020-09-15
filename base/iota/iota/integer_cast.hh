/*
	integer_cast.hh
	---------------
*/

#ifndef IOTA_INTEGERCAST_HH
#define IOTA_INTEGERCAST_HH


namespace iota
{
	
	template < class Out, class In, class Fail >
	inline
	Out integer_cast( In i, Fail fail )
	{
		const bool input_type_has_sign  = In ( -1 ) < 0;
		const bool output_type_has_sign = Out( -1 ) < 0;
		
		if ( input_type_has_sign == output_type_has_sign )
		{
			if ( sizeof (In) > sizeof (Out) )
			{
				if ( Out( i ) != i )
				{
					fail();
				}
			}
		}
		else if ( output_type_has_sign )
		{
			if ( sizeof (In) >= sizeof (Out) )
			{
				if ( Out( i ) < 0  ||  Out( i ) != i )
				{
					fail();
				}
			}
		}
		else if ( i < 0 )
		{
			fail();
		}
		
		return Out( i );
	}
	
}

#endif
