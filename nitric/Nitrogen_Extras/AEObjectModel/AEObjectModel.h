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

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	inline Nucleus::Owned< AEDesc_Token > GetRootToken()
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
	
	Nucleus::Owned< AEDesc_Token > DispatchPropertyAccess( AEObjectClass        desiredClass,
	                                                       const AEDesc_Token&  containerToken,
	                                                       AEObjectClass        containerClass,
	                                                       AEEnumerated         keyForm,
	                                                       const AEDesc_Data&   keyData,
	                                                       RefCon );
	
	Nucleus::Owned< AEDesc_Token > DispatchAccessToList( AEObjectClass        desiredClass,
	                                                     const AEDesc_Token&  containerToken,
	                                                     AEObjectClass        containerClass,
	                                                     AEEnumerated         keyForm,
	                                                     const AEDesc_Data&   keyData,
	                                                     RefCon );
	
}

#endif
