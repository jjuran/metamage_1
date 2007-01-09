/*	========
 *	Count.cp
 *	========
 */

#ifndef AEOBJECTMODEL_COUNT_H
#include "AEObjectModel/Count.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	std::size_t Count( AEObjectClass   desiredClass,
	                   AEObjectClass   containerClass,
	                   const AEToken&  containerToken )
	{
		return TheGlobalCounter().Count( desiredClass, containerClass, containerToken );
	}
	
	std::size_t Counter::Count( AEObjectClass   desiredClass,
	                            AEObjectClass   containerClass,
	                            const AEToken&  containerToken )
	{
		Map::const_iterator found = map.find( Key( desiredClass, DescType( containerToken.descriptorType ) ) );
		
		if ( found == map.end() )
		{
			found = map.find( Key( AEObjectClass( typeWildCard ), DescType( containerToken.descriptorType ) ) );
			
			if ( found == map.end() )
			{
				found = map.find( Key( desiredClass, typeWildCard ) );
				
				if ( found == map.end() )
				{
					found = map.find( Key( AEObjectClass( typeWildCard ), typeWildCard ) );
					
					if ( found == map.end() )
					{
						throw ErrAEEventNotHandled();
					}
				}
			}
		}
		
		return found->second( desiredClass, containerClass, containerToken );
	}
	
	Counter& TheGlobalCounter()
	{
		static Counter theGlobalCounter;
		
		return theGlobalCounter;
	}
	
}

