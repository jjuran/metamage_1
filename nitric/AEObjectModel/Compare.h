/*	=========
 *	Compare.h
 *	=========
 */

#ifndef AEOBJECTMODEL_COMPARE_H
#define AEOBJECTMODEL_COMPARE_H

#include <map>

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AECOMPOPERATOR_HH
#include "Mac/AppleEvents/Types/AECompOperator.hh"
#endif

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


namespace Nitrogen
{
	
	bool Compare( Mac::AECompOperator       op,
			      const Mac::AEDesc_Token&  obj1,
			      const Mac::AEDesc_Token&  obj2 );
	
	template < Mac::DescType tokenType > struct Compare_Traits;
	
	class Comparer
	{
		public:
			typedef bool (*Callback)( Mac::AECompOperator, const Mac::AEDesc_Token&, const Mac::AEDesc_Token& );
		
		private:
			typedef std::map< Mac::DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			Comparer( const Comparer& );
			Comparer& operator=( const Comparer& );
		
		public:
			Comparer()  {}
			
			void Register( Mac::DescType tokenType, Comparer::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < Mac::DescType tokenType >
			void Register()
			{
				Register( tokenType, Compare_Traits< tokenType >::Compare );
			}
			
			bool Compare( Mac::AECompOperator       op,
			              const Mac::AEDesc_Token&  obj1,
			              const Mac::AEDesc_Token&  obj2 );
	};
	
	Comparer& TheGlobalComparer();
	
	inline void RegisterComparer( Mac::DescType tokenType, Comparer::Callback callback )
	{
		TheGlobalComparer().Register( tokenType, callback );
	}
	
	template < Mac::DescType tokenType >
	inline void RegisterComparer()
	{
		TheGlobalComparer().template Register< tokenType >();
	}
	
	template < Mac::DescType descType >
	struct BasicComparer
	{
		typedef typename DescType_result< descType >::type Result;
		
		static bool Compare( Mac::AECompOperator       op,
		                     const Mac::AEDesc_Token&  obj1,
		                     const Mac::AEDesc_Token&  obj2 )
		{
			if ( op != Mac::AECompOperator( kAEEquals ) )
			{
				Mac::ThrowOSStatus( errAEEventNotHandled );
			}
			
			Result a = AEGetDescData< descType >( obj1 );
			Result b = AEGetDescData< descType >( obj2 );
			
			return a == b;
		}
	};
	
	template < Mac::DescType descType >
	struct NumericComparer
	{
		typedef typename DescType_result< descType >::type Result;
		
		static bool Compare( Mac::AECompOperator       op,
		                     const Mac::AEDesc_Token&  obj1,
		                     const Mac::AEDesc_Token&  obj2 )
		{
			Result a = AEGetDescData< descType >( obj1 );
			Result b = AEGetDescData< descType >( obj2 );
			
			switch ( op )
			{
				case kAEEquals:
					return a == b;
				
				case kAEGreaterThan:
					return a > b;
				
				case kAEGreaterThanEquals:
					return a >= b;
				
				case kAELessThan:
					return a < b;
				
				case kAELessThanEquals:
					return a <= b;
				
				default:
					break;
			}
			
			Mac::ThrowOSStatus( errAEEventNotHandled );
		}
	};
	
}

#endif
