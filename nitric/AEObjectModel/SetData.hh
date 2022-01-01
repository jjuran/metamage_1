/*
	SetData.hh
	----------
*/

#ifndef AEOBJECTMODEL_SETDATA_HH
#define AEOBJECTMODEL_SETDATA_HH

// Standard C++
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
			
			void SetData( const Mac::AEDesc_Token& obj, const Mac::AEDesc_Data& data );
	};
	
	DataSetter& TheGlobalDataSetter();
	
	inline void RegisterDataSetter( Mac::DescType tokenType, DataSetter::Callback callback )
	{
		TheGlobalDataSetter().Register( tokenType, callback );
	}
	
}

#endif
