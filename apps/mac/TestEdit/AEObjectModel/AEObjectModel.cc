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
DispatchAccessToList( Mac::AEObjectClass        desiredClass,
                      const Mac::AEDesc_Token&  containerToken,
                      Mac::AEObjectClass        containerClass,
                      Mac::AEKeyForm            keyForm,
                      const Mac::AEDesc_Data&   keyData )
{
	nucleus::owned< Mac::AEDescList_Token > result = AECreateList< Mac::AEDescList_Token >();
	
	OSErr err;
	
	/*
		Given a query like `get the name of every document`,
		an object accessor will be called to produce a list
		of tokens for each document.  This object accessor,
		in turn, will be called to access the name from the
		list.  We accomplish this by retrieving each token
		from the list and calling the object accessor that
		matches the token type of an individual list item
		and the same key data that we were given (in this
		example, the `name` property), for each item.
		
		The subtoken's AEDesc, but not its token-nature,
		will be disposed automatically, which is what we
		want.  We explicitly dispose of the new token's
		AEDesc after adding it the the list (if it was
		successfully created).  If adding it to the list
		fails, then we destroy the object's token-nature
		as well.  In the event of failure, we explicitly
		throw an exception and the partially populated
		token list is destroyed in full.
	*/
	
	long count = AECountItems( containerToken );
	
	for ( long i = 1;  i <= count;  ++i )
	{
		nucleus::owned< Mac::AEDesc_Data > subtoken = AEGetNthDesc( containerToken, i );
		
		AEDesc object;
		
		err = AECallObjectAccessor( desiredClass,
		                            &subtoken.get(),
		                            containerClass,
		                            keyForm,
		                            &keyData,
		                            &object );
		
		if ( err == noErr )
		{
			err = AEPutDesc( &Detail::mutable_AEDesc( result ), 0, &object );
			
			if ( err )
			{
				AEDisposeToken( &object );
			}
			else
			{
				::AEDisposeDesc( &object );
			}
		}
		
		Mac::ThrowOSStatus( err );
	}
	
	return result;
}

}
