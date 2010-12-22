/*	===============
 *	AEObjectModel.h
 *	===============
 */

#ifndef AEOBJECTMODEL_AEOBJECTMODEL_H
#define AEOBJECTMODEL_AEOBJECTMODEL_H

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	inline nucleus::owned< Mac::AEDesc_Token > GetRootToken()
	{
		return nucleus::owned< Mac::AEDesc_Token >();
	}
	
	UInt32 ComputeAbsoluteIndex( const Mac::AEDesc_Data&  keyData,
	                             std::size_t              count );
	
	pascal OSErr OSLCompare( ::DescType     op,
	                         const AEDesc*  obj1,
	                         const AEDesc*  obj2,
	                         ::Boolean*     result );
	
	pascal OSErr OSLCount( ::DescType     desiredClass,
	                       ::DescType     containerClass,
	                       const AEDesc*  containerToken,
	                       SInt32*        result );
	
	pascal OSErr OSLDisposeToken( AEDesc* token );
	
	void AESetObjectCallbacks();
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	DispatchPropertyAccess( Mac::AEObjectClass        desiredClass,
	                        const Mac::AEDesc_Token&  containerToken,
	                        Mac::AEObjectClass        containerClass,
	                        Mac::AEKeyForm            keyForm,
	                        const Mac::AEDesc_Data&   keyData,
	                        RefCon );
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	DispatchAccessToList( Mac::AEObjectClass        desiredClass,
	                      const Mac::AEDesc_Token&  containerToken,
	                      Mac::AEObjectClass        containerClass,
	                      Mac::AEKeyForm            keyForm,
	                      const Mac::AEDesc_Data&   keyData,
	                      RefCon );
	
}

#endif
