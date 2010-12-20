/*
	nucleus/seizing_POD_scribe.hh
	-----------------------------
*/

#ifndef NUCLEUS_SEIZINGPODSCRIBE_HH
#define NUCLEUS_SEIZINGPODSCRIBE_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif


namespace nucleus
{
	
	template < class Ownable >
	struct seizing_POD_scribe
	{
		typedef Ownable Put_Parameter;
		
		template < class Putter >
		static void Put( Put_Parameter param, const Putter& putter )
		{
			putter( &param, &param + 1 );
		}
		
		typedef ::nucleus::owned< Ownable > Get_Result;
		
		template < class Getter >
		static Get_Result Get( const Getter& getter )
		{
			Get_Result result;
			
			getter( &result, &result + 1 );
			
			return ::nucleus::owned< Ownable >::seize( result );
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result  Result;
		
		static const bool hasStaticSize = true;
		
		typedef Ownable Buffer;
	};
	
}

#endif

