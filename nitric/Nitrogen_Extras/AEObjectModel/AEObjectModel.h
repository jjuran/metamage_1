/*	===============
 *	AEObjectModel.h
 *	===============
 */

#ifndef AEOBJECTMODEL_AEOBJECTMODEL_H
#define AEOBJECTMODEL_AEOBJECTMODEL_H

#include <map>

#include <AERegistry.h>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	inline Owned< AEToken, AETokenDisposer > GetRootToken()
	{
		return AEInitializeToken();
	}
	
	UInt32 ComputeAbsoluteIndex( const AEDesc&  keyData,
	                             std::size_t    count );
	
	#pragma mark -
	#pragma mark ¥ OSL Object Callbacks ¥
	
	pascal OSErr OSLCompare( ::DescType      op,
	                         const AEToken*  obj1,
	                         const AEToken*  obj2,
	                         ::Boolean*      result );
	
	pascal OSErr OSLCount( ::DescType      desiredClass,
	                       ::DescType      containerClass,
	                       const AEToken*  containerToken,
	                       SInt32*         result );
	
	pascal OSErr OSLDisposeToken( AEToken* token );
	
	void AESetObjectCallbacks();
	
	Owned< AEToken, AETokenDisposer > DispatchPropertyAccess( AEObjectClass   desiredClass,
	                                                          const AEToken&  containerToken,
	                                                          AEObjectClass   containerClass,
	                                                          AEEnumerated    keyForm,
	                                                          const AEDesc&   keyData,
	                                                          RefCon );
	
	Owned< AEToken, AETokenDisposer > DispatchAccessToList( AEObjectClass   desiredClass,
	                                                        const AEToken&  containerToken,
	                                                        AEObjectClass   containerClass,
	                                                        AEEnumerated    keyForm,
	                                                        const AEDesc&   keyData,
	                                                        RefCon );
	
}

#endif
