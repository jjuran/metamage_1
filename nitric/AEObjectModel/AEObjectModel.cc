/*
	AEObjectModel.cc
	----------------
*/

#include "AEObjectModel/AEObjectModel.hh"

// Standard C
#include <stdlib.h>

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif
#ifndef NITROGEN_ASREGISTRY_HH
#include "Nitrogen/ASRegistry.hh"
#endif

// Nitrogen Extras / AEObjectModel
#include "AEObjectModel/AccessProperty.hh"


namespace Nitrogen
{
	
	static
	OSErr dispose_token_list( AEDesc& list )
	{
		OSErr err;
		long  count;
		
		err = AECountItems( &list, &count );
		
		if ( err )
		{
			return err;
		}
		
		for ( long i = count;  i > 0;  --i )
		{
			AEDesc    token;
			AEKeyword keyword;
			
			err = AEGetNthDesc( &list, i, typeWildCard, &keyword, &token );
			
			if ( err )
			{
				break;
			}
			
			AEDeleteItem( &list, i );
			
			AEDisposeToken( &token );
		}
		
		return err;
	}
	
	static
	OSErr dispose_token( AEDesc& token )
	{
		// FIXME:  Check for typeWildCard?
		
		OSErr err = noErr;
		
		if ( token.descriptorType == typeAEList )
		{
			err = dispose_token_list( token );
		}
		
		::AEDisposeDesc( &token );
		
		return err;
	}
	
	/*
		Given key data of form formAbsolutePosition and a count of the elements,
		ComputeAbsoluteIndex() will calculate the corresponding index.
		
		A return value of zero indicates that all elements were specified (kAEAll),
		which is the only case that doesn't throw if the count is zero.
		
		For kAEMiddle, the item before the midpoint is chosen if count is even.
		
		For kAEAny, rand() is used.  A scripter who has specialized random
		number generation requirements should generate those numbers elsewhere
		and use typeSInt32 instead.
		
		For literal index data, -1 refers to the last element, -2 to the second-to-last,
		etc.  Any index (including zero) that doesn't correspond to an existing element
		will cause errAENoSuchObject to be thrown.
	*/
	
	UInt32 ComputeAbsoluteIndex( const Mac::AEDesc_Data&  keyData,
	                             std::size_t              count )
	{
		switch ( keyData.descriptorType )
		{
			case typeAbsoluteOrdinal:
				{
					Mac::AEAbsoluteOrdinal ordinal = AEGetDescData< Mac::typeAbsoluteOrdinal >( keyData );
					
					// Check for 'every' first
					if ( ordinal == Mac::kAEAll )
					{
						return 0;
					}
					else
					{
						// Anything else requires a non-empty list
						if ( count == 0 )
						{
							Mac::ThrowOSStatus( errAENoSuchObject );
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
							return UInt32( rand() / (RAND_MAX + 1.0) * count ) + 1;
						
						default:
							break;
					}
				}
				break;
			
			case typeSInt32:
				{
					SInt32 index = AEGetDescData< Mac::typeSInt32 >( keyData );
					
					if ( index < 0 )
					{
						// e.g. count == 10, index == -2 -> index = 9
						index = count + (index + 1);
					}
					
					if ( index > 0  &&  index <= count )
					{
						return index;
					}
				}
				
				break;
			
			default:
				break;
		}
		
		Mac::ThrowOSStatus( errAENoSuchObject );
		
		// Not reached
		return 0;
	}
	
	pascal OSErr OSLDisposeToken( AEDesc* token )
	{
		return dispose_token( *token );
	}
	
	void AESetObjectCallbacks()
	{
		Nitrogen::AESetObjectCallbacks( (::OSLCompareProcPtr     ) NULL,
		                                (::OSLCountProcPtr       ) NULL,
		                                OSLDisposeToken,
		                                (::OSLGetMarkTokenProcPtr) NULL,
		                                (::OSLMarkProcPtr        ) NULL,
		                                (::OSLAdjustMarksProcPtr ) NULL,
		                                (::OSLGetErrDescProcPtr  ) NULL );
	}
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	DispatchPropertyAccess( Mac::AEObjectClass        desiredClass,
	                        const Mac::AEDesc_Token&  containerToken,
	                        Mac::AEObjectClass        containerClass,
	                        Mac::AEKeyForm            keyForm,
	                        const Mac::AEDesc_Data&   keyData )
	{
		if ( keyData.descriptorType == typeAbsoluteOrdinal )
		{
			return AccessAllProperties( containerToken, containerClass );
		}
		
		Mac::AEPropertyID propertyID = Mac::AEPropertyID( AEGetDescData< Mac::typeType >( keyData ) + 0 );
		
		return AccessProperty( propertyID, containerToken, containerClass );
	}
	
	static inline const Mac::AEDesc_Token& AEToken_Cast( const AEDesc& token )
	{
		return AEDesc_Cast< const Mac::AEDesc_Token >( token );
	}
	
	class ObjectAccessor_Caller
	{
		private:
			Mac::AEObjectClass       itsDesiredClass;
			Mac::AEObjectClass       itsContainerClass;
			Mac::AEKeyForm           itsKeyForm;
			const Mac::AEDesc_Data&  itsKeyData;
		
		public:
			ObjectAccessor_Caller( Mac::AEObjectClass       desiredClass,
	                               Mac::AEObjectClass       containerClass,
	                               Mac::AEKeyForm           keyForm,
	                               const Mac::AEDesc_Data&  keyData )
			:
				itsDesiredClass  ( desiredClass   ),
				itsContainerClass( containerClass ),
				itsKeyForm       ( keyForm        ),
				itsKeyData       ( keyData        )
			{
			}
			
			nucleus::owned< Mac::AEDesc_Token > operator()( const AEDesc& containerToken ) const
			{
				try
				{
					return AECallObjectAccessor( itsDesiredClass,
					                             AEToken_Cast( containerToken ),
					                             itsContainerClass,
					                             itsKeyForm,
					                             itsKeyData );
				}
				catch ( const Mac::OSStatus& err )
				{
					if ( err != errAENoSuchObject )
					{
						throw;
					}
					
					return MissingValue();
				}
			}
	};
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	DispatchAccessToList( Mac::AEObjectClass        desiredClass,
	                      const Mac::AEDesc_Token&  containerToken,
	                      Mac::AEObjectClass        containerClass,
	                      Mac::AEKeyForm            keyForm,
	                      const Mac::AEDesc_Data&   keyData )
	{
		nucleus::owned< Mac::AEDescList_Token > result = AECreateList< Mac::AEDescList_Token >();
		
		ObjectAccessor_Caller caller( desiredClass,
		                              containerClass,
		                              keyForm,
		                              keyData );
		
		long count = AECountItems( containerToken );
		
		for ( long i = 1;  i <= count;  ++i )
		{
			AEPutDesc( result, 0, caller( AEGetNthDesc( containerToken, i ) ) );
		}
		
		return result;
	}
	
}
