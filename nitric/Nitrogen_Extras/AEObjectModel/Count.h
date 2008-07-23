/*	=======
 *	Count.h
 *	=======
 */

#ifndef AEOBJECTMODEL_COUNT_H
#define AEOBJECTMODEL_COUNT_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	std::size_t Count( AEObjectClass        desiredClass,
			           AEObjectClass        containerClass,
			           const AEDesc_Token&  containerToken );
	
	template < ::DescType desiredClass, ::DescType containerType > struct Count_Traits;
	
	class Counter
	{
		public:
			typedef std::size_t (*Callback)( AEObjectClass, AEObjectClass, const AEDesc_Token& );
		
		private:
			class Key
			{
				private:
					AEObjectClass desiredClass;
					DescType      containerType;
				
				public:
					Key()  {}
					Key( AEObjectClass desiredClass, DescType containerType )
					:
						desiredClass ( desiredClass  ),
						containerType( containerType )
					{}
					
					friend bool operator<( const Key& a, const Key& b )
					{
						return a.desiredClass < b.desiredClass || a.desiredClass == b.desiredClass
						                                       && a.containerType < b.containerType;
					}
			};
			
			typedef std::map< Key, Callback >  Map;
			
			Map map;
			
			// not implemented:
			Counter( const Counter& );
			Counter& operator=( const Counter& );
		
		public:
			Counter()  {}
			
			void Register( AEObjectClass desiredClass, DescType containerType, Counter::Callback callback )
			{
				map[ Key( desiredClass, containerType ) ] = callback;
			}
			
			template < ::DescType desiredClass, ::DescType containerType >
			void Register()
			{
				Register( desiredClass, containerType, Count_Traits< desiredClass, containerType >::Count );
			}
			
			std::size_t Count( AEObjectClass        desiredClass,
			                   AEObjectClass        containerClass,
			                   const AEDesc_Token&  containerToken );
	};
	
	Counter& TheGlobalCounter();
	
	inline void RegisterCounter( AEObjectClass desiredClass, DescType containerType, Counter::Callback callback )
	{
		TheGlobalCounter().Register( desiredClass, containerType, callback );
	}
	
	template < ::DescType desiredClass, ::DescType containerType >
	inline void RegisterCounter()
	{
		TheGlobalCounter().template Register< desiredClass, containerType >();
	}
	
}

#endif
