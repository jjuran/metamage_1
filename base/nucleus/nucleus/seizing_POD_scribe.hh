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
		typedef                   Ownable    argument_type;
		typedef ::nucleus::owned< Ownable >  result_type;
		
		template < class Putter >
		static void Put( argument_type param, const Putter& putter )
		{
			putter( &param, &param + 1 );
		}
		
		template < class Getter >
		static result_type Get( const Getter& getter )
		{
			result_type result;
			
			getter( &result, &result + 1 );
			
			return ::nucleus::owned< Ownable >::seize( result );
		}
		
		static const bool hasStaticSize = true;
		
		typedef Ownable Buffer;
		
		static const std::size_t static_size = sizeof (Ownable);
	};
	
}

#endif

