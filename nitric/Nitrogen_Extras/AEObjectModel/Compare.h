/*	=========
 *	Compare.h
 *	=========
 */

#ifndef AEOBJECTMODEL_COMPARE_H
#define AEOBJECTMODEL_COMPARE_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	bool Compare( AECompOperator  op,
			      const AEToken&  obj1,
			      const AEToken&  obj2 );
	
	template < ::DescType tokenType > struct Compare_Traits;
	
	class Comparer
	{
		public:
			typedef bool (*Callback)( AECompOperator, const AEToken&, const AEToken& );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			Comparer( const Comparer& );
			Comparer& operator=( const Comparer& );
		
		public:
			Comparer()  {}
			
			void Register( DescType tokenType, Comparer::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, Compare_Traits< tokenType >::Compare );
			}
			
			bool Compare( AECompOperator  op,
			              const AEToken&  obj1,
			              const AEToken&  obj2 );
	};
	
	Comparer& TheGlobalComparer();
	
	inline void RegisterComparer( DescType tokenType, Comparer::Callback callback )
	{
		TheGlobalComparer().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterComparer()
	{
		TheGlobalComparer().template Register< tokenType >();
	}
	
}

#endif
