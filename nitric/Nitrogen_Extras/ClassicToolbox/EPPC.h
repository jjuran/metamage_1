// EPPC.h

#ifndef CLASSICTOOLBOX_EPPC_H
#define CLASSICTOOLBOX_EPPC_H

// Universal Interfaces
#ifndef __EPPC__
#include <EPPC.h>
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_PPCTOOLBOX_H
#include "ClassicToolbox/PPCToolbox.h"
#endif


namespace Nitrogen
{
	
	void RegisterHighLevelEventManagerErrors();
	
	using ::TargetID;
	
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
	
#if CALL_NOT_IN_CARBON
	
#endif
	
}

#endif
