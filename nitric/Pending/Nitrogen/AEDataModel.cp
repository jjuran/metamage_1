// AEDataModel.cp

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_SCOPED_H
#include "Nitrogen/Scoped.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

namespace Nitrogen {
	
	void RegisterAppleEventManagerErrors()
	{
		RegisterOSStatus< paramErr                  >();
		RegisterOSStatus< eLenErr                   >();
		RegisterOSStatus< memFullErr                >();
		RegisterOSStatus< userCanceledErr           >();
		RegisterOSStatus< procNotFound              >();
		RegisterOSStatus< bufferIsSmall             >();
		RegisterOSStatus< noOutstandingHLE          >();
		RegisterOSStatus< connectionInvalid         >();
		RegisterOSStatus< noUserInteractionAllowed  >();
		RegisterOSStatus< noPortErr                 >();
		RegisterOSStatus< destPortErr               >();
		RegisterOSStatus< sessClosedErr             >();
		RegisterOSStatus< errAECoercionFail         >();
		RegisterOSStatus< errAEDescNotFound         >();
		RegisterOSStatus< errAECorruptData          >();
		RegisterOSStatus< errAEWrongDataType        >();
		RegisterOSStatus< errAENotAEDesc            >();
		RegisterOSStatus< errAEBadListItem          >();
		RegisterOSStatus< errAENewerVersion         >();
		RegisterOSStatus< errAENotAppleEvent        >();
		RegisterOSStatus< errAEEventNotHandled      >();
		RegisterOSStatus< errAEReplyNotValid        >();
		RegisterOSStatus< errAEUnknownSendMode      >();
		RegisterOSStatus< errAEWaitCanceled         >();
		RegisterOSStatus< errAETimeout              >();
		RegisterOSStatus< errAENoUserInteraction    >();
		RegisterOSStatus< errAENotASpecialFunction  >();
		RegisterOSStatus< errAEParamMissed          >();
		RegisterOSStatus< errAEUnknownAddressType   >();
		RegisterOSStatus< errAEHandlerNotFound      >();
		RegisterOSStatus< errAEReplyNotArrived      >();
		RegisterOSStatus< errAEIllegalIndex         >();
		RegisterOSStatus< errAEImpossibleRange      >();
		RegisterOSStatus< errAEWrongNumberArgs      >();
		RegisterOSStatus< errAEAccessorNotFound     >();
		RegisterOSStatus< errAENoSuchLogical        >();
		RegisterOSStatus< errAEBadTestKey           >();
		RegisterOSStatus< errAENotAnObjSpec         >();
		RegisterOSStatus< errAENoSuchObject         >();
		RegisterOSStatus< errAENegativeCount        >();
		RegisterOSStatus< errAEEmptyListContainer   >();
		RegisterOSStatus< errAEUnknownObjectType    >();
		RegisterOSStatus< errAERecordingIsAlreadyOn >();
	}
	
	AECoercionHandler::AECoercionHandler()
	:
		fromType(),
		toType(),
		handler(),
		handlerRefCon(),
		fromTypeIsDesc(),
		isSysHandler()
	{}
	
	bool operator==( const AECoercionHandler& a, const AECoercionHandler& b )
	{
		return a.handler        == b.handler
		    && a.handlerRefCon  == b.handlerRefCon
		    && a.fromType       == b.fromType
		    && a.toType         == b.toType
		    && a.fromTypeIsDesc == b.fromTypeIsDesc
		    && a.isSysHandler   == b.isSysHandler;
	}
	
	Owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall )
	{
		ThrowOSStatus
		(
			::AEInstallCoercionHandler
			(
				toInstall.fromType, 
				toInstall.toType, 
				toInstall.handler, 
				toInstall.handlerRefCon, 
				toInstall.fromTypeIsDesc,
				toInstall.isSysHandler
			)
		);
		return Owned< AECoercionHandler >::Seize( toInstall );
	}
	
	AECoercionHandler AEGetCoercionHandler
	(
		DescType fromType,
		DescType toType,
		bool isSysHandler
	)
	{
		::AECoercionHandlerUPP handler;
		long handlerRefCon;
		::Boolean fromTypeIsDesc;
		
		ThrowOSStatus
		(
			::AEGetCoercionHandler
			(
				fromType,
				toType,
				&handler,
				&handlerRefCon,
				&fromTypeIsDesc,
				isSysHandler
			)
		);
		
		return AECoercionHandler( fromType, toType, handler, handlerRefCon, fromTypeIsDesc, isSysHandler );
	}
	
	Owned< AEDesc > AECoercePtr( DescType typeCode, const void* dataPtr, Size dataSize, DescType toType )
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		// Necessary for OS 9; OS X does this automatically
		if ( toType == TypeWildCard() )
		{
			toType = typeCode;
		}
		
		AEDesc result;
		ThrowOSStatus( ::AECoercePtr( typeCode, dataPtr, dataSize, toType, &result ) );
		return Owned< AEDesc >::Seize( result );
	}
	
	Owned< AEDesc > AECoerceDesc( const AEDesc& desc, DescType toType )
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		// Necessary for OS 9; OS X does this automatically
		if ( toType == TypeWildCard() )
		{
			toType = DescType( desc.descriptorType );
		}
		
		AEDesc result;
		ThrowOSStatus( ::AECoerceDesc( &desc, toType, &result ) );
		return Owned< AEDesc >::Seize( result );
	}
	
	class AEDescEditor
	{
		private:
			Owned< AEDesc >& desc;
			AEDesc workingCopy;
		public:
			AEDescEditor( Owned< AEDesc >& desc ) : desc( desc ), workingCopy( desc.Release() )  {}
			~AEDescEditor()  { desc = Owned< AEDesc >::Seize( workingCopy ); }
			
			AEDesc& Get()  { return workingCopy; }
			operator AEDesc&()  { return Get(); }
	};
	
	// Mac OS X allows AERecord arguments to have any descriptorType.
	// Mac OS 9 does not, so we fake this behavior by adjusting the descriptorType
	// prior to calling the function.
	// We need two ways of doing this -- one for const AEDesc& and one for non-const.
	// For const (AEGetKey{Ptr,Desc}, AESizeOfKeyDesc), we call Fix_AERecordDescriptorType
	// to return an adjusted AERecord, a pointer to which is passed to the function.
	// For non-const (AEPutKey{Ptr,Desc}, AEDeleteKeyDesc), we use Scoped< ::DescType& >,
	// which adjusts the actual AEDesc argument that was passed, and afterward adjusts it back.
	
	static AERecord Fix_AERecordDescriptorType( AERecord record )
	{
		record.descriptorType = typeAERecord;
		return record;
	}
	
	Owned< AEDesc > AECreateDesc(DescType typeCode, const void* dataPtr, Size dataSize)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		if ( dataPtr == NULL )
		{
			throw ParamErr();
		}
		
		AEDesc desc;
		ThrowOSStatus( ::AECreateDesc(typeCode, dataPtr, dataSize, &desc) );
		return Owned< AEDesc >::Seize(desc);
	}
	
	Owned< AEDesc > AECreateDesc(DescType typeCode, ::Handle handle)
	{
		if ( handle == NULL )
		{
			throw ParamErr();
		}
		
		// FIXME:  Replace with Scoped<> when verified as working
		//SavedHandleState savedHandleState( handle );
		
		Owned< AEDesc > result;
		SInt8 state = ::HGetState( handle );
		try
		{
			::HLock( handle );
			result = AECreateDesc(typeCode, *handle, ::GetHandleSize( handle ) );
			::HSetState( handle, state );
		}
		catch ( ... )
		{
			::HSetState( handle, state );
			throw;
		}
		return result;
	}
	
	Owned< AEDesc > AECreateDesc( DescType typeCode, Owned< Handle > handle )
	{
	#if TARGET_API_MAC_CARBON
		return AECreateDesc( typeCode, handle.Get() );
	#else
		AEDesc desc;
		desc.descriptorType = typeCode;
		desc.dataHandle = handle.Release();
		return Owned< AEDesc >::Seize( desc );
	#endif
	}
	
	Owned< AEDesc > AECreateDesc(DescType typeCode, Owned< AEDesc > desc)
	{
		AEDescEditor( desc ).Get().descriptorType = typeCode;
		return desc;
	}
	
	Owned< AEDesc > AEDuplicateDesc( const AEDesc& desc )
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AEDesc result;
		ThrowOSStatus( ::AEDuplicateDesc( &desc, &result ) );
		return Owned< AEDesc >::Seize( result );
	}
	
	Owned< AEDesc > AECreateList( bool isRecord )
	{
		return AECreateList(NULL, 0, isRecord);
	}
	
	Owned< AEDesc > AECreateList(const void* factoringPtr, std::size_t factoredSize, bool isRecord)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AEDesc desc;
		ThrowOSStatus( ::AECreateList(factoringPtr, factoredSize, isRecord, &desc) );
		return Owned< AEDesc >::Seize(desc);
	}
	
	long AECountItems(const AEDesc& desc)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		long count;
		ThrowOSStatus( ::AECountItems(&desc, &count) );
		return count;
	}
	
	void AEPutPtr( AEDescList& list, long index, DescType type, const void* dataPtr, Size dataSize )
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus(
			::AEPutPtr( &list, index, type, dataPtr, dataSize )
		);
	}
	
	void AEPutPtr( Owned< AEDescList >& list, long index, DescType type, const void* dataPtr, Size dataSize )
	{
		AEPutPtr( AEDescEditor( list ), index, type, dataPtr, dataSize );
	}
	
	void AEPutDesc( AEDescList& list, long index, const AEDesc& desc )
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus(
			::AEPutDesc( &list, index, &desc )
		);
	}
	
	void AEPutDesc( Owned< AEDescList >& list, long index, const AEDesc& desc )
	{
		AEPutDesc( AEDescEditor( list ), index, desc );
	}
	
	GetNthPtr_Result AEGetNthPtr(
		const AEDesc& listDesc, 
		long index, 
		DescType desiredType, 
		void* dataPtr, 
		Size maximumSize) 
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		::AEKeyword keyword;
		::DescType typeCode;
		::Size actualSize;
		
		ThrowOSStatus
		(
			::AEGetNthPtr
			(
				&listDesc, 
				index, 
				desiredType, 
				&keyword, 
				&typeCode, 
				dataPtr, 
				maximumSize, 
				&actualSize
			)
		);
		
		GetNthPtr_Result result;
		result.keyword    = AEKeyword( keyword );
		result.typeCode   = DescType( typeCode );
		result.actualSize = actualSize;
		
		return result;
	}
	
	Owned< AEDesc > AEGetNthDesc(
		const AEDesc& listDesc, 
		long index, 
		DescType desiredType, 
		::AEKeyword* keywordResult
	)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		::AEKeyword keyword;
		AEDesc result;
		
		ThrowOSStatus(
			::AEGetNthDesc(
				&listDesc, 
				index, 
				desiredType, 
				keywordResult ? keywordResult : &keyword, 
				&result
			)
		);
		return Owned< AEDesc >::Seize(result);
	}
	
	Owned< AEDesc > AEGetNthDesc(
		const AEDesc& listDesc, 
		long index, 
		::AEKeyword* keywordResult
	)
	{
		return AEGetNthDesc(
			listDesc, 
			index, 
			TypeWildCard(), 
			keywordResult
		);
	}
	
	Size AESizeOfNthItem(const AEDescList& list, long index)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus
		(
			::AESizeOfNthItem
			(
				&list, 
				index, 
				&typeCode, 
				&dataSize
			)
		);
		
		AEGetParamPtr_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteItem( AEDescList& list, long index )
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus( ::AEDeleteItem( &list, index ) );
	}
	
	void AEDeleteItem( Owned< AEDescList >& list, long index )
	{
		AEDeleteItem( AEDescEditor( list ), index );
	}
	
	void AEPutKeyPtr(
		AERecord& record, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		Scoped< ::DescType& > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus(
			::AEPutKeyPtr( &record, keyword, typeCode, dataPtr, dataSize )
		);
	}
	
	void AEPutKeyPtr(
		Owned< AERecord >& record, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		AEPutKeyPtr( AEDescEditor( record ), keyword, typeCode, dataPtr, dataSize );
	}
	
	void AEPutKeyDesc(
		AERecord& record, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		Scoped< ::DescType& > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEPutKeyDesc( &record, keyword, &desc ) );
	}
	
	void AEPutKeyDesc(
		Owned< AERecord >& record, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		AEPutKeyDesc( AEDescEditor( record ), keyword, desc );
	}
	
	AEGetKeyPtr_Result AEGetKeyPtr(
		const AERecord& record, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus(
			::AEGetKeyPtr(
				&fixedAERecord, 
				keyword, 
				desiredType, 
				&typeCode, 
				dataPtr, 
				maximumSize, 
				&dataSize
			)
		);
		
		AEGetKeyPtr_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	Owned< AEDesc > AEGetKeyDesc(
		const AERecord& record, 
		AEKeyword keyword, 
		DescType desiredType)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
		
		AEDesc result;
		ThrowOSStatus(
			::AEGetKeyDesc(
				&fixedAERecord, 
				keyword, 
				desiredType, 
				&result
			)
		);
		return Owned< AEDesc >::Seize( result );
	}
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc(
		const AERecord& record, 
		AEKeyword keyword)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus
		(
			::AESizeOfKeyDesc
			(
				&fixedAERecord, 
				keyword, 
				&typeCode, 
				&dataSize
			)
		);
		
		AESizeOfKeyDesc_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteKeyDesc(
		AERecord& record, 
		AEKeyword keyword)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		Scoped< ::DescType& > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEDeleteKeyDesc( &record, keyword ) );
	}
	
	void AEDeleteKeyDesc(
		Owned< AERecord >& record, 
		AEKeyword keyword)
	{
		AEDeleteKeyDesc( AEDescEditor( record ), keyword );
	}
	
	Owned< AppleEvent > AECreateAppleEvent(
		AEEventClass eventClass, 
		AEEventID eventID, 
		const AEAddressDesc& target, 
		AEReturnID returnID, 
		AETransactionID transactionID)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AppleEvent result;
		ThrowOSStatus(
			::AECreateAppleEvent(
				eventClass, 
				eventID, 
				&target, 
				returnID, 
				transactionID, 
				&result
			)
		);
		return Owned< AppleEvent >::Seize( result );
	}
	
	void AEPutParamPtr(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus(
			::AEPutParamPtr(
				&appleEvent, 
				keyword, 
				typeCode, 
				dataPtr, 
				dataSize
			)
		);
	}
	
	void AEPutParamPtr(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		AEPutParamPtr(
			AEDescEditor( appleEvent ), 
			keyword, 
			typeCode, 
			dataPtr, 
			dataSize
		);
	}
	
	void AEPutParamDesc(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus(
			::AEPutParamDesc( &appleEvent, keyword, &desc )
		);
	}
	
	void AEPutParamDesc(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		AEPutParamDesc( AEDescEditor( appleEvent ), keyword, desc );
	}
	
	AEGetParamPtr_Result AEGetParamPtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus
		(
			::AEGetParamPtr
			(
				&appleEvent, 
				keyword, 
				desiredType, 
				&typeCode, 
				dataPtr, 
				maximumSize, 
				&dataSize
			)
		);
		
		AEGetParamPtr_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	Owned< AEDesc > AEGetParamDesc(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AEDesc result;
		ThrowOSStatus(
			::AEGetParamDesc(
				&appleEvent, 
				keyword, 
				desiredType, 
				&result
			)
		);
		return Owned< AEDesc >::Seize( result );
	}
	
	AESizeOfParam_Result AESizeOfParam(
		const AppleEvent& appleEvent, 
		AEKeyword keyword)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus
		(
			::AESizeOfParam
			(
				&appleEvent, 
				keyword, 
				&typeCode, 
				&dataSize
			)
		);
		
		AESizeOfParam_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteParam(
		AppleEvent& appleEvent, 
		AEKeyword keyword)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus( ::AEDeleteParam( &appleEvent, keyword ) );
	}
	
	void AEDeleteParam(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword)
	{
		AEDeleteParam( AEDescEditor( appleEvent ), keyword );
	}
	
	void AEPutAttributePtr(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus(
			::AEPutAttributePtr(
				&appleEvent, 
				keyword, 
				typeCode, 
				dataPtr, 
				dataSize
			)
		);
	}
	
	void AEPutAttributePtr(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		AEPutAttributePtr(
			AEDescEditor( appleEvent ), 
			keyword, 
			typeCode, 
			dataPtr, 
			dataSize
		);
	}
	
	void AEPutAttributeDesc(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus(
			::AEPutAttributeDesc( &appleEvent, keyword, &desc )
		);
	}
	
	void AEPutAttributeDesc(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		AEPutAttributeDesc( AEDescEditor( appleEvent ), keyword, desc );
	}
	
	AEGetAttributePtr_Result AEGetAttributePtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus
		(
			::AEGetAttributePtr
			(
				&appleEvent, 
				keyword, 
				desiredType, 
				&typeCode, 
				dataPtr, 
				maximumSize, 
				&dataSize
			)
		);
		
		AEGetAttributePtr_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	Owned< AEDesc > AEGetAttributeDesc(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AEDesc result;
		ThrowOSStatus(
			::AEGetAttributeDesc(
				&appleEvent, 
				keyword, 
				desiredType, 
				&result
			)
		);
		return Owned< AEDesc >::Seize( result );
	}
	
	AESizeOfAttribute_Result AESizeOfAttribute(
		const AppleEvent& appleEvent, 
		AEKeyword keyword)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus
		(
			::AESizeOfAttribute
			(
				&appleEvent, 
				keyword, 
				&typeCode, 
				&dataSize
			)
		);
		
		AESizeOfAttribute_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEGetDescData( const AEDesc& desc, void* dataPtr, std::size_t maximumSize )
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
		
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		
		ThrowOSStatus( ::AEGetDescData( &desc, dataPtr, maximumSize ) );
		
	#else
		
		::BlockMoveData( *desc.dataHandle, dataPtr, maximumSize );
		
	#endif
	}
	
	void AEReplaceDescData(
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize, 
		AEDesc& result)
	{
		OnlyOnce< RegisterAppleEventManagerErrors >();
	
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		
		ThrowOSStatus
		(
			::AEReplaceDescData( typeCode, dataPtr, dataSize, &result )
		);
		
	#else
		
		bool typeIsNull = typeCode == typeNull;
		bool ptrIsNull  = dataPtr  == NULL;
		
		if ( typeIsNull != ptrIsNull )
		{
			throw ParamErr();
		}
		
		bool descWasNull = result.dataHandle == NULL;
		
		if ( !descWasNull && !ptrIsNull )
		{
			SetHandleSize( result.dataHandle, dataSize );
			::BlockMoveData( dataPtr, *result.dataHandle, dataSize );
			result.descType = typeCode;
		}
		else if ( descWasNull && !ptrIsNull )
		{
			result = AECreateDesc( typeCode, dataPtr, dataSize ).Release();
		}
		else if ( !descWasNull )
		{
			::AEDisposeDesc( result );
		}
		
	#endif
	}
	
	void AEReplaceDescData(
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize, 
		Owned< AEDesc >& result)
	{
		AEReplaceDescData( typeCode, dataPtr, dataSize, AEDescEditor( result ) );
	}
	
}

