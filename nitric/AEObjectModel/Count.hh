/*
	Count.hh
	--------
*/

#ifndef AEOBJECTMODEL_COUNT_HH
#define AEOBJECTMODEL_COUNT_HH

// Standard C++
#include <map>

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEDESC_HH
#include "Mac/AppleEvents/Types/AEDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif


namespace Nitrogen
{
	
	std::size_t Count( Mac::AEObjectClass        desiredClass,
			           Mac::AEObjectClass        containerClass,
			           const Mac::AEDesc_Token&  containerToken );
	
	class Counter
	{
		public:
			typedef std::size_t (*Callback)( Mac::AEObjectClass, Mac::AEObjectClass, const Mac::AEDesc_Token& );
		
		private:
			class Key
			{
				private:
					Mac::AEObjectClass  desiredClass;
					Mac::DescType       containerType;
				
				public:
					Key()  {}
					Key( Mac::AEObjectClass desiredClass, Mac::DescType containerType )
					:
						desiredClass ( desiredClass  ),
						containerType( containerType )
					{}
					
					friend bool operator<( const Key& a, const Key& b )
					{
						return  a.desiredClass <  b.desiredClass
						    || (a.desiredClass == b.desiredClass  &&  a.containerType < b.containerType);
					}
			};
			
			typedef std::map< Key, Callback >  Map;
			
			Map map;
			
			// not implemented:
			Counter( const Counter& );
			Counter& operator=( const Counter& );
		
		public:
			Counter()  {}
			
			void Register( Mac::AEObjectClass desiredClass, Mac::DescType containerType, Counter::Callback callback )
			{
				map[ Key( desiredClass, containerType ) ] = callback;
			}
			
			std::size_t Count( Mac::AEObjectClass        desiredClass,
			                   Mac::AEObjectClass        containerClass,
			                   const Mac::AEDesc_Token&  containerToken );
	};
	
	Counter& TheGlobalCounter();
	
	inline void RegisterCounter( Mac::AEObjectClass desiredClass, Mac::DescType containerType, Counter::Callback callback )
	{
		TheGlobalCounter().Register( desiredClass, containerType, callback );
	}
	
}

#endif
