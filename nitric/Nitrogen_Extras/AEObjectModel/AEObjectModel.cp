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
	#pragma mark ¥ GetObjectClass ¥
	
	ObjectClassGetter::ObjectClassGetter()
	{
		Register< typeNull >();
	}
	
	AEObjectClass ObjectClassGetter::GetObjectClass( const AEToken& obj )
	{
		Map::const_iterator found = map.find( obj.descriptorType );
		if ( found == map.end() )
		{
			//throw ErrAEEventNotHandled();
			return obj.descriptorType;
		}
		
		return found->second( obj );
	}
	
	ObjectClassGetter& TheGlobalObjectClassGetter()
	{
		static ObjectClassGetter theGlobalObjectClassGetter;
		return theGlobalObjectClassGetter;
	}
	
	AEObjectClass GetObjectClass( const AEToken& obj )
	{
		return TheGlobalObjectClassGetter().GetObjectClass( obj );
	}
	
	#pragma mark -
	#pragma mark ¥ GetData ¥
	
	DataGetter::DataGetter()
	{
		Register< typeNull >();
	}
	
	Owned< AEDesc > DataGetter::GetData( const AEToken& obj, DescType desiredType )
	{
		Map::const_iterator found = map.find( obj.descriptorType );
		if ( found == map.end() )
		{
			throw ErrAEEventNotHandled();
		}
		
		return found->second( obj, desiredType );
	}
	
	DataGetter& TheGlobalDataGetter()
	{
		static DataGetter theGlobalDataGetter;
		return theGlobalDataGetter;
	}
	
	Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType )
	{
		return TheGlobalDataGetter().GetData( obj, desiredType );
	}
	
	#pragma mark -
	#pragma mark ¥ SetData ¥
	
	DataSetter::DataSetter()
	{
	}
	
	void DataSetter::SetData( const AEToken& obj, const AEDesc& data )
	{
		Map::const_iterator found = map.find( obj.descriptorType );
		if ( found == map.end() )
		{
			throw ErrAEEventNotHandled();
		}
		
		return found->second( obj, data );
	}
	
	DataSetter& TheGlobalDataSetter()
	{
		static DataSetter theGlobalDataSetter;
		return theGlobalDataSetter;
	}
	
	void SetData( const AEToken& obj, const AEDesc& data )
	{
		return TheGlobalDataSetter().SetData( obj, data );
	}
	
	#pragma mark -
	#pragma mark ¥ Compare ¥
	
	bool Comparer::Compare( AECompOperator  op,
	                        const AEToken&  obj1,
	                        const AEToken&  obj2 )
	{
		Map::const_iterator found = map.find( obj1.descriptorType );
		if ( found == map.end() )
		{
			found = map.find( obj2.descriptorType );
			if ( found == map.end() )
			{
				throw ErrAEEventNotHandled();
			}
		}
		
		return found->second( op, obj1, obj2 );
	}
	
	Comparer& TheGlobalComparer()
	{
		static Comparer theGlobalComparer;
		return theGlobalComparer;
	}
	
	bool Compare( AECompOperator  op,
	              const AEToken&  obj1,
	              const AEToken&  obj2 )
	{
		return TheGlobalComparer().Compare( op, obj1, obj2 );
	}
	
	#pragma mark -
	#pragma mark ¥ Count ¥
	
	std::size_t Counter::Count( AECompOperator  desiredClass,
	                            AEObjectClass   containerClass,
	                            const AEToken&  containerToken )
	{
		Map::const_iterator found = map.find( Key( desiredClass, containerToken.descriptorType ) );
		if ( found == map.end() )
		{
			found = map.find( Key( typeWildCard, containerToken.descriptorType ) );
			if ( found == map.end() )
			{
				found = map.find( Key( desiredClass, typeWildCard ) );
				if ( found == map.end() )
				{
					found = map.find( Key( typeWildCard, typeWildCard ) );
					if ( found == map.end() )
					{
						throw ErrAEEventNotHandled();
					}
				}
			}
		}
		
		return found->second( desiredClass, containerClass, containerToken );
	}
	
	Counter& TheGlobalCounter()
	{
		static Counter theGlobalCounter;
		return theGlobalCounter;
	}
	
	std::size_t Count( AEObjectClass   desiredClass,
	                   AEObjectClass   containerClass,
	                   const AEToken&  containerToken )
	{
		return TheGlobalCounter().Count( desiredClass, containerClass, containerToken );
	}
	
	#pragma mark -
	#pragma mark ¥ DisposeToken ¥
	
	TokenDisposer::TokenDisposer()
	{
		Register< typeAEList >();
	}
	
	void TokenDisposer::DisposeToken( Owned< AEToken > token )
	{
		Map::const_iterator found = map.find( token.Get().descriptorType );
		if ( found == map.end() )
		{
			// If we omitted this, the descriptor would still be disposed
			// at function exit.
			
			AEDisposeDesc( token );
			
			return;  // FIXME:  Check for typeWildCard
		}
		
		return found->second( token );
	}
	
	TokenDisposer& TheGlobalTokenDisposer()
	{
		static TokenDisposer theGlobalTokenDisposer;
		return theGlobalTokenDisposer;
	}
	
	void DisposeToken( Owned< AEToken > token )
	{
		return TheGlobalTokenDisposer().DisposeToken( token );
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

