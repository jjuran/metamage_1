/*	==========
 *	Compare.cp
 *	==========
 */

#ifndef AEOBJECTMODEL_COMPARE_H
#include "AEObjectModel/Compare.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	bool Compare( AECompOperator  op,
	              const AEToken&  obj1,
	              const AEToken&  obj2 )
	{
		return TheGlobalComparer().Compare( op, obj1, obj2 );
	}
	
	bool Comparer::Compare( AECompOperator  op,
	                        const AEToken&  obj1,
	                        const AEToken&  obj2 )
	{
		Map::const_iterator found = map.find( obj1.descriptorType );
		if ( found == map.end() )
		{
			found = map.find( obj2.descriptorType );
			if ( found == map.end() )
			{
				throw ErrAEEventNotHandled();
			}
		}
		
		return found->second( op, obj1, obj2 );
	}
	
	Comparer& TheGlobalComparer()
	{
		static Comparer theGlobalComparer;
		return theGlobalComparer;
	}
	
}

