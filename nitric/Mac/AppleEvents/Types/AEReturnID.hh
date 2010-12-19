/*
	Mac/AppleEvents/Types/AEReturnID.hh
	-----------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AERETURNID_HH
#define MAC_APPLEEVENTS_TYPES_AERETURNID_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AEReturnID
	{
		kAutoGenerateReturnID = ::kAutoGenerateReturnID,
		
		kAEReturnID_Max = nucleus::enumeration_traits< ::AEReturnID >::max
	};
	
	// The OSA runtime can generate 'long' return IDs greater than 0x7fff.
	// AECreateAppleEvent() can only ever accept SInt16, but our specialization
	// of AEGetAttributePtr() needs to use a 32-bit type.
	
	enum AEReturnID_32Bit
	{
		kAEReturnID_32Bit_Max = nucleus::enumeration_traits< ::SInt32 >::max
	};
	
}

#endif

