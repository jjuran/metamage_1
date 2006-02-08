/*	================
 *	AEObjectModel.cp
 *	================
 */

#ifndef AEOBJECTMODEL_AEOBJECTMODEL_H
#include "AEObjectModel/AEObjectModel.h"
#endif

// Nitrogen
#ifndef NITROGEN_ASREGISTRY_H
#include "Nitrogen/ASRegistry.h"
#endif

// Nitrogen Extras / AEObjectModel
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

// Nitrogen Extras / Iteration
#ifndef ITERATION_AEDESCLISTITEMS_H
#include "Iteration/AEDescListItems.h"
#endif

// Nitrogen Extras / Templates
#ifndef TEMPLATES_POINTERTOFUNCTION_H
#include "Templates/TrappingFunction.h"
#endif


namespace Nitrogen
{
	
	// Given key data of form formAbsolutePosition and a count of the elements,
	// ComputeAbsoluteIndex() will calculate the corresponding index.
	// 
	// A return value of zero indicates that all elements were specified (kAEAll),
	// which is the only case that doesn't throw if the count is zero.
	// 
	// For kAEMiddle, the item before the midpoint is chosen if count is even.
	// 
	// For kAEAny, std::rand() is used.  A scripter who has specialized random
	// number generation requirements should generate those numbers elsewhere
	// and use typeSInt32 instead.
	// 
	// For literal index data, -1 refers to the last element, -2 to the second-to-last,
	// etc.  Any index (including zero) that doesn't correspond to an existing element
	// will cause errAENoSuchObject to be thrown.
	
	UInt32 ComputeAbsoluteIndex( const AEDesc&  keyData,
	                             std::size_t    count )
	{
		AEEnumerated ordinal;
		SInt32 index;
		
		switch ( keyData.descriptorType )
		{
			case typeAbsoluteOrdinal:
				ordinal = AEGetDescData< typeEnumerated >( keyData );
				
				// Check for 'every' first
				if ( ordinal == kAEAll )
				{
					return 0;
				}
				else
				{
					// Anything else requires a non-empty list
					if ( count == 0 )
					{
						throw ErrAENoSuchObject();
					}
				}
				
				switch ( ordinal )
				{
					case kAEFirst:
						return 1;
					case kAELast:
						return count;
					case kAEMiddle:
						// If there's an even number of elements,
						// this picks the one before the midpoint.
						return (count + 1) / 2;
					case kAEAny:
						return UInt32( std::rand() / (RAND_MAX + 1.0) * count ) + 1;
					default:
						break;
				}
				break;
			
			case typeSInt32:
				index = AEGetDescData< typeSInt32 >( keyData );
				
				if ( index < 0 )
				{
					// e.g. count == 10, index == -2 -> index = 9
					index = count + (index + 1);
				}
				
				if ( index > 0  &&  index < count )
				{
					return index;
				}
				
				break;
			
			default:
				break;
		}
		
		throw ErrAENoSuchObject();
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
			DisposeToken( Nucleus::Owned< AEDesc >::Seize( *token ) );
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
	
	Nucleus::Owned< AEToken, AETokenDisposer > DispatchPropertyAccess( AEObjectClass   desiredClass,
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
	
	static Nucleus::Owned< AEToken, AETokenDisposer > CallObjectAccessor( const AEToken& containerToken,
	                                                                      const ObjectAccessContext& context )
	{
		return AECallObjectAccessor( context.desiredClass,
		                             containerToken,
		                             context.containerClass,
		                             context.keyForm,
		                             context.keyData );
	}
	
	static Nucleus::Owned< AEToken, AETokenDisposer > CallObjectAccessorWithContext( const AEToken& containerToken,
	                                                                                 const ObjectAccessContext& context )
	{
		return CallObjectAccessor( containerToken,
		                           context );
	}
	
	Nucleus::Owned< AEToken, AETokenDisposer > DispatchAccessToList( AEObjectClass   desiredClass,
	                                                                 const AEToken&  containerToken,
	                                                                 AEObjectClass   containerClass,
	                                                                 AEEnumerated    keyForm,
	                                                                 const AEDesc&   keyData,
	                                                                 RefCon )
	{
		Nucleus::Owned< AEDescList, AETokenDisposer > result = AECreateTokenList();
		
		AEDescList_ItemValue_Container values = AEDescList_ItemValues( containerToken );
		
		std::transform( values.begin(),
		                values.end(),
		                AEDescList_Item_BackInserter( result ),
		                Trap1( std::bind2nd( PtrFun( CallObjectAccessorWithContext ),
		                                     ObjectAccessContext( desiredClass,
		                                                          containerClass,
		                                                          keyForm,
		                                                          keyData ) ),
		                       TrapException( ErrAENoSuchObject(),
		                                      PtrFun( MissingValue ) ) ) );
		
		return result;
	}
	
}

