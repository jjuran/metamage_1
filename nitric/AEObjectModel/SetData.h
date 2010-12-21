/*	=========
 *	SetData.h
 *	=========
 */

#ifndef AEOBJECTMODEL_SETDATA_H
#define AEOBJECTMODEL_SETDATA_H

#include <map>

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEDESC_HH
#include "Mac/AppleEvents/Types/AEDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif


namespace Nitrogen
{
	
	void SetData( const Mac::AEDesc_Token& obj, const Mac::AEDesc_Data& data );
	
	template < ::DescType tokenType > struct SetData_Traits;
	
	class DataSetter
	{
		public:
			typedef void (*Callback)( const Mac::AEDesc_Token&, const Mac::AEDesc_Data& );
		
		private:
			typedef std::map< Mac::DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			DataSetter( const DataSetter& );
			DataSetter& operator=( const DataSetter& );
		
		public:
			DataSetter();
			
			void Register( Mac::DescType tokenType, DataSetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, SetData_Traits< tokenType >::SetData );
			}
			
			void SetData( const Mac::AEDesc_Token& obj, const Mac::AEDesc_Data& data );
	};
	
	DataSetter& TheGlobalDataSetter();
	
	inline void RegisterDataSetter( Mac::DescType tokenType, DataSetter::Callback callback )
	{
		TheGlobalDataSetter().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	inline void RegisterDataSetter()
	{
		TheGlobalDataSetter().template Register< tokenType >();
	}
	
}

#endif
