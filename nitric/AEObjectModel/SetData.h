/*	=========
 *	SetData.h
 *	=========
 */

#ifndef AEOBJECTMODEL_SETDATA_H
#define AEOBJECTMODEL_SETDATA_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ SetData ¥
	
	void SetData( const AEDesc_Token& obj, const AEDesc_Data& data );
	
	template < ::DescType tokenType > struct SetData_Traits;
	
	class DataSetter
	{
		public:
			typedef void (*Callback)( const AEDesc_Token&, const AEDesc_Data& );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			DataSetter( const DataSetter& );
			DataSetter& operator=( const DataSetter& );
		
		public:
			DataSetter();
			
			void Register( DescType tokenType, DataSetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, SetData_Traits< tokenType >::SetData );
			}
			
			void SetData( const AEDesc_Token& obj, const AEDesc_Data& data );
	};
	
	DataSetter& TheGlobalDataSetter();
	
	inline void RegisterDataSetter( DescType tokenType, DataSetter::Callback callback )
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
