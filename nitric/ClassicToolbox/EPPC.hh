// EPPC.hh

#ifndef CLASSICTOOLBOX_EPPC_HH
#define CLASSICTOOLBOX_EPPC_HH

// Mac OS
#ifndef __EPPC__
#include <EPPC.h>
#endif

// nucleus
#include "nucleus/errors_registered.hh"

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_PPCTOOLBOX_HH
#include "ClassicToolbox/PPCToolbox.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( HighLevelEventManager );
	
	
	using ::TargetID;
	
}

namespace nucleus
{
	
	template <>  struct maker< TargetID >
	{
		TargetID operator()( const PPCPortRec& name, const LocationNameRec& location ) const
		{
			TargetID target;
			
			target.sessionID = 0;
			target.name = name;
			target.location = location;
			//target.recvrName;
			
			return target;
		}
		
		TargetID operator()( const PPCPortRec& name ) const
		{
			return make< TargetID >( name, make< LocationNameRec >() );
		}
	};
	
}

namespace Nitrogen
{
	
}

#endif
