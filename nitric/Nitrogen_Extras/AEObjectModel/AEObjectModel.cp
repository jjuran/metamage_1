/*	================
 *	AEObjectModel.cp
 *	================
 */

#ifndef AEOBJECTMODEL_AEOBJECTMODEL_H
#include "AEObjectModel/AEObjectModel.h"
#endif

#ifndef NITROGEN_AEDESCLISTITEMS_H
#include "Nitrogen/AEDescListItems.h"
#endif

#ifndef NITROGEN_POINTERTOFUNCTION_H
#include "Nitrogen/PointerToFunction.h"
#endif

#ifndef AEOBJECTMODEL_ACCESSPROPERTY_H
#include "AEObjectModel/AccessProperty.h"
#endif
#ifndef AEOBJECTMODEL_COMPARE_H
#include "AEObjectModel/Compare.h"
#endif
#ifndef AEOBJECTMODEL_COUNT_H
#include "AEObjectModel/Count.h"
#endif
#ifndef AEOBJECTMODEL_DISPOSETOKEN_H
#include "AEObjectModel/DisposeToken.h"
#endif


namespace Nitrogen
{
	
	Owned< AEObjectSpecifier > AECreateObjectSpecifier( AEObjectClass             objectClass,
	                                                    const AEObjectSpecifier&  container,
	                                                    AEEnumeration             keyForm,
	                                                    const AEDesc&             keyData )
	{
		return AECoerceDesc
		(
			AECreateList< true >()
				<< keyAEDesiredClass + AECreateDesc< typeType       >( DescType( objectClass ) )
				<< keyAEKeyForm      + AECreateDesc< typeEnumerated >(              keyForm    )
				<< keyAEKeyData      + keyData
				<< keyAEContainer    + container,
				
			typeObjectSpecifier
		);
	}
	
	#pragma mark -
	#pragma mark ¥ OSL Object Callbacks ¥
	
	pascal OSErr OSLCompare( ::DescType      op,
	                         const AEToken*  obj1,
	                         const AEToken*  obj2,
	                         ::Boolean*      result )
	{
		try
		{
			*result = Compare( op, *obj1, *obj2 );
		}
		catch ( OSStatus err )
		{
			return err;
		}
		
		return noErr;
	}
	
	pascal OSErr OSLCount( ::DescType      desiredClass,
	                       ::DescType      containerClass,
	                       const AEToken*  containerToken,
	                       SInt32*         result )
	{
		try
		{
			*result = Count( desiredClass, containerClass, *containerToken );
		}
		catch ( OSStatus err )
		{
			return err;
		}
		
		return noErr;
	}
	
	pascal OSErr OSLDisposeToken( AEToken* token )
	{
		try
		{
			// Yes, that's right.  We are seizing the token with Disposer< AEDesc >, 
			// not AETokenDisposer.  That's because we're releasing the token-ness,
			// but not the AEDesc-ness just yet.
			// More pragmatically, it makes the called code work more smoothly.
			// The called function is only responsible for disposing the token material,
			// and the Owned destructor will call AEDisposeDesc() for us.
			
			// Atom or list, it will hence be missed.
			DisposeToken( Owned< AEDesc >::Seize( *token ) );
		}
		catch ( OSStatus err )
		{
			return err;
		}
		
		return noErr;
	}
	
	void AESetObjectCallbacks()
	{
		Nitrogen::AESetObjectCallbacks( OSLCompare,
		                                OSLCount,
		                                OSLDisposeToken,
		                                ( ::OSLGetMarkTokenProcPtr )NULL,
		                                ( ::OSLMarkProcPtr         )NULL,
		                                ( ::OSLAdjustMarksProcPtr  )NULL,
		                                ( ::OSLGetErrDescProcPtr   )NULL );
	}
	
	Owned< AEToken, AETokenDisposer > DispatchPropertyAccess( AEObjectClass   desiredClass,
	                                                          const AEToken&  containerToken,
	                                                          AEObjectClass   containerClass,
	                                                          AEEnumerated    keyForm,
	                                                          const AEDesc&   keyData,
	                                                          RefCon )
	{
		
		AEPropertyID propertyID = AEGetDescData< typeType >( keyData );
		
		return AccessProperty( propertyID, containerToken, containerClass );
	}
	
	struct ObjectAccessContext
	{
		AEObjectClass  desiredClass;
		AEObjectClass  containerClass;
		AEEnumerated   keyForm;
		const AEDesc&  keyData;
		
		ObjectAccessContext( AEObjectClass  desiredClass,
		                     AEObjectClass  containerClass,
		                     AEEnumerated   keyForm,
		                     const AEDesc&  keyData )
		:
			desiredClass  ( desiredClass ),
			containerClass( containerClass ),
			keyForm       ( keyForm ),
			keyData       ( keyData )
		{}
	};
	
	static Owned< AEToken, AETokenDisposer > CallObjectAccessor( const AEToken& containerToken,
	                                                             const ObjectAccessContext& context )
	{
		return AECallObjectAccessor( context.desiredClass,
		                             containerToken,
		                             context.containerClass,
		                             context.keyForm,
		                             context.keyData );
	}
	
	static Owned< AEToken, AETokenDisposer > CallObjectAccessor_( Const_AEDescList_Item containerToken,
	                                                              const ObjectAccessContext& context )
	{
		return CallObjectAccessor( containerToken.Get(),
		                           context );
	}
	
	Owned< AEToken, AETokenDisposer > DispatchAccessToList( AEObjectClass   desiredClass,
	                                                        const AEToken&  containerToken,
	                                                        AEObjectClass   containerClass,
	                                                        AEEnumerated    keyForm,
	                                                        const AEDesc&   keyData,
	                                                        RefCon )
	{
		/*
		Owned< AEDescList > result = AECreateList< false >();
		
		std::transform( AEDescList_Items( containerToken ).begin(),
		                AEDescList_Items( containerToken ).end(),
		                AEDescList_Items( result ).begin(),
		                std::bind2nd( PtrFun( CallObjectAccessor_ ),
		                              ObjectAccessContext( desiredClass,
		                                                   containerClass,
		                                                   keyForm,
		                                                   keyData ) ) );
		
		return Owned< AEToken, AETokenDisposer >::Seize( result.Release() );
		*/
		
		typedef Const_AEDescList_Item_Container::const_iterator const_iterator;
		
		Owned< AEDescList, AETokenDisposer > result;
		result = Owned< AEToken, AETokenDisposer >::Seize( AECreateList< false >().Release() );
		
		for ( const_iterator it = AEDescList_Items( containerToken ).begin();
		      it != AEDescList_Items( containerToken ).end();
		      ++it )
		{
			AEPutDesc( AETokenEditor( result ),
			           0,
			           AECallObjectAccessor( desiredClass,
			                                 (*it).Get(),
			                                 containerClass,
			                                 keyForm,
			                                 keyData ) );
		}
		return result;
	}
	
}

