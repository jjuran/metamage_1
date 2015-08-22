/*
	dyad.hh
	-------
*/

#ifndef VC_DYAD_HH
#define VC_DYAD_HH

// plus
#include "plus/integer.hh"

// vc
#include "vc/op_type.hh"


namespace vc
{
	
	struct dyad
	{
		typedef plus::integer integer;
		
		integer  i;
		op_type  op;
		
		dyad( op_type op ) : op( op )
		{
		}
		
		dyad( const integer& i ) : i( i ), op()
		{
		}
		
		void swap( dyad& b );
	};
	
	inline void swap( dyad& a, dyad& b )
	{
		a.swap( b );
	}
	
}

#endif
