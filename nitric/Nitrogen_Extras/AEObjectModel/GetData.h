/*	=========
 *	GetData.h
 *	=========
 */

#ifndef AEOBJECTMODEL_GETDATA_H
#define AEOBJECTMODEL_GETDATA_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	inline Nucleus::Owned< AEObjectSpecifier > GetRootObjectSpecifier()
	{
		return AEInitializeDesc();
	}
	
	Nucleus::Owned< AEObjectSpecifier > AECreateObjectSpecifier( AEObjectClass             objectClass,
	                                                             const AEObjectSpecifier&  container,
	                                                             AEEnumeration             keyForm,
	                                                             const AEDesc&             keyData );
	
	Nucleus::Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType = typeWildCard );
	
	class DataGetter
	{
		public:
			typedef Nucleus::Owned< AEDesc > (*Callback)( const AEToken&, DescType );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			DataGetter( const DataGetter& );
			DataGetter& operator=( const DataGetter& );
		
		public:
			DataGetter();
			
			void Register( DescType tokenType, DataGetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			Nucleus::Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType );
	};
	
	DataGetter& TheGlobalDataGetter();
	
	inline void RegisterDataGetter( DescType tokenType, DataGetter::Callback callback )
	{
		TheGlobalDataGetter().Register( tokenType, callback );
	}
	
}

#endif
