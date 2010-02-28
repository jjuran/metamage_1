// EPPC.hh

#ifndef CLASSICTOOLBOX_EPPC_HH
#define CLASSICTOOLBOX_EPPC_HH

// Mac OS
#ifndef __EPPC__
#include <EPPC.h>
#endif

// Nucleus
#include "Nucleus/ErrorsRegistered.h"

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_PPCTOOLBOX_HH
#include "ClassicToolbox/PPCToolbox.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( HighLevelEventManager );
	
	
	using ::TargetID;
	
}

namespace Nucleus
{
	
	template <>  struct Maker< TargetID >
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
			return Make< TargetID >( name, Make< LocationNameRec >() );
		}
	};
	
}

namespace Nitrogen
{
	
}

#endif
