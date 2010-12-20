/*	=======
 *	Count.h
 *	=======
 */

#ifndef AEOBJECTMODEL_COUNT_H
#define AEOBJECTMODEL_COUNT_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	std::size_t Count( Mac::AEObjectClass   desiredClass,
			           Mac::AEObjectClass   containerClass,
			           const AEDesc_Token&  containerToken );
	
	template < ::DescType desiredClass, ::DescType containerType > struct Count_Traits;
	
	class Counter
	{
		public:
			typedef std::size_t (*Callback)( Mac::AEObjectClass, Mac::AEObjectClass, const AEDesc_Token& );
		
		private:
			class Key
			{
				private:
					Mac::AEObjectClass  desiredClass;
					DescType            containerType;
				
				public:
					Key()  {}
					Key( Mac::AEObjectClass desiredClass, DescType containerType )
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
			
			void Register( Mac::AEObjectClass desiredClass, DescType containerType, Counter::Callback callback )
			{
				map[ Key( desiredClass, containerType ) ] = callback;
			}
			
			template < ::DescType desiredClass, ::DescType containerType >
			void Register()
			{
				Register( desiredClass, containerType, Count_Traits< desiredClass, containerType >::Count );
			}
			
			std::size_t Count( Mac::AEObjectClass   desiredClass,
			                   Mac::AEObjectClass   containerClass,
			                   const AEDesc_Token&  containerToken );
	};
	
	Counter& TheGlobalCounter();
	
	inline void RegisterCounter( Mac::AEObjectClass desiredClass, DescType containerType, Counter::Callback callback )
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
