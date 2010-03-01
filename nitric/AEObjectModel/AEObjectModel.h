/*	===============
 *	AEObjectModel.h
 *	===============
 */

#ifndef AEOBJECTMODEL_AEOBJECTMODEL_H
#define AEOBJECTMODEL_AEOBJECTMODEL_H

#include <map>

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	inline nucleus::owned< AEDesc_Token > GetRootToken()
	{
		return AEInitializeDesc< AEDesc_Token >();
	}
	
	UInt32 ComputeAbsoluteIndex( const AEDesc_Data&  keyData,
	                             std::size_t         count );
	
	#pragma mark -
	#pragma mark ¥ OSL Object Callbacks ¥
	
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
	
	nucleus::owned< AEDesc_Token > DispatchPropertyAccess( AEObjectClass        desiredClass,
	                                                       const AEDesc_Token&  containerToken,
	                                                       AEObjectClass        containerClass,
	                                                       AEEnumerated         keyForm,
	                                                       const AEDesc_Data&   keyData,
	                                                       RefCon );
	
	nucleus::owned< AEDesc_Token > DispatchAccessToList( AEObjectClass        desiredClass,
	                                                     const AEDesc_Token&  containerToken,
	                                                     AEObjectClass        containerClass,
	                                                     AEEnumerated         keyForm,
	                                                     const AEDesc_Data&   keyData,
	                                                     RefCon );
	
}

#endif
