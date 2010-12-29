/*	==========
 *	Compare.cp
 *	==========
 */

#ifndef AEOBJECTMODEL_COMPARE_H
#include "AEObjectModel/Compare.h"
#endif


namespace Nitrogen
{
	
	bool Compare( Mac::AECompOperator       op,
	              const Mac::AEDesc_Token&  obj1,
	              const Mac::AEDesc_Token&  obj2 )
	{
		return TheGlobalComparer().Compare( op, obj1, obj2 );
	}
	
	bool Comparer::Compare( Mac::AECompOperator       op,
	                        const Mac::AEDesc_Token&  obj1,
	                        const Mac::AEDesc_Token&  obj2 )
	{
		Map::const_iterator found = map.find( Mac::DescType( obj1.descriptorType ) );
		
		if ( found == map.end() )
		{
			found = map.find( Mac::DescType( obj2.descriptorType ) );
			
			if ( found == map.end() )
			{
				ThrowOSStatus( errAEEventNotHandled );
			}
		}
		
		return found->second( Mac::AECompOperator( op ), obj1, obj2 );
	}
	
	Comparer& TheGlobalComparer()
	{
		static Comparer theGlobalComparer;
		
		return theGlobalComparer;
	}
	
}

