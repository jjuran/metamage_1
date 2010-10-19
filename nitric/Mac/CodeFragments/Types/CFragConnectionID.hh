/*
	Mac/CodeFragments/Types/CFragConnectionID.hh
	--------------------------------------------
*/

#ifndef MAC_CODEFRAGMENTS_TYPES_CFRAGCONNECTIONID_HH
#define MAC_CODEFRAGMENTS_TYPES_CFRAGCONNECTIONID_HH

#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

// nucleus
#include "nucleus/disposer.hh"


namespace nucleus
{
	
	template <>
	struct disposer< CFragConnectionID >
	{
		typedef CFragConnectionID  argument_type;
		typedef void               result_type;
		
		void operator()( CFragConnectionID connID ) const
		{
			::CloseConnection( &connID );
		}
	};
	
}

#endif

