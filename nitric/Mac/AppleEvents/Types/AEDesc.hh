/*
	Mac/AppleEvents/Types/AEDesc.hh
	-------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEDESC_HH
#define MAC_APPLEEVENTS_TYPES_AEDESC_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif


namespace Mac
{
	
	struct AEDesc_Data : AEDesc
	{
	};
	
	typedef AEDesc_Data AEDescList_Data, AERecord_Data;
	typedef AEDesc_Data AEAddressDesc, AppleEvent;
	typedef AEDesc_Data AEDesc_ObjectSpecifier;
	
	struct AEDesc_Token : AEDesc
	{
	};
	
	typedef AEDesc_Token AEDescList_Token, AERecord_Token;
	
	inline bool operator==( const AEDesc& a, const AEDesc& b )
	{
		return    a.descriptorType == b.descriptorType
		       && a.dataHandle     == b.dataHandle;
	}
	
	inline bool operator!=( const AEDesc& a, const AEDesc& b )
	{
		return !( a == b );
	}
	
}

#endif

