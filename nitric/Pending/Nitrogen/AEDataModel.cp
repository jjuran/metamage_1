// AEDataModel.cp

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen {
	
	class AEDescEditor
	{
		private:
			Owned<AEDesc>& desc;
			AEDesc workingCopy;
		public:
			AEDescEditor( Owned<AEDesc>& desc ) : desc( desc ), workingCopy( desc.Release() )  {}
			~AEDescEditor()  { desc = Owned<AEDesc>::Seize( workingCopy ); }
			
			AEDesc& Get()  { return workingCopy; }
			operator AEDesc&()  { return Get(); }
	};
	
	void RegisterAppleEventManagerErrors()
	{
		using RegisterOSStatus;
		
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
	
	Owned<AEDesc> AECoercePtr(DescType typeCode, const void* dataPtr, Size dataSize, DescType toType)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc result;
		ThrowOSStatus( ::AECoercePtr(typeCode, dataPtr, dataSize, toType, &result) );
		return Owned<AEDesc>::Seize(result);
	}
	
	Owned<AEDesc> AECoerceDesc(const AEDesc& desc, DescType toType)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc result;
		ThrowOSStatus( ::AECoerceDesc(&desc, toType, &result) );
		return Owned<AEDesc>::Seize(result);
	}
	
	Owned<AEDesc> AECreateDesc(DescType typeCode, const void* dataPtr, Size dataSize)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc desc;
		ThrowOSStatus( ::AECreateDesc(typeCode, dataPtr, dataSize, &desc) );
		return Owned<AEDesc>::Seize(desc);
	}
	
	Owned<AEDesc> AECreateDesc(DescType typeCode, ::Handle handle)
	{
		// FIXME:  Replace with Scoped<> when verified as working
		//SavedHandleState savedHandleState( handle );
		
		Owned<AEDesc> result;
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
	
	Owned<AEDesc> AECreateDesc(DescType typeCode, Owned< Handle > handle)
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
	
	Owned<AEDesc> AECreateDesc(DescType typeCode, Owned< AEDesc > desc)
	{
		AEDescEditor(desc).Get().descriptorType = typeCode;
		return desc;
	}
	
	Owned<AEDesc> AEDuplicateDesc( const AEDesc& desc )
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc result;
		ThrowOSStatus( ::AEDuplicateDesc(&desc, &result) );
		return Owned<AEDesc>::Seize( result );
	}
	
	Owned<AEDesc> AECreateList(bool isRecord)
	{
		return AECreateList(NULL, 0, isRecord);
	}
	
	Owned<AEDesc> AECreateList(const void* factoringPtr, std::size_t factoredSize, bool isRecord)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc desc;
		ThrowOSStatus( ::AECreateList(factoringPtr, factoredSize, isRecord, &desc) );
		return Owned<AEDesc>::Seize(desc);
	}
	
	long AECountItems(const AEDesc& desc)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		long count;
		ThrowOSStatus( ::AECountItems(&desc, &count) );
		return count;
	}
	
	void AEPutPtr(Owned<AEDescList>& list, long index, DescType type, const void* dataPtr, Size dataSize) 
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutPtr(
				&AEDescEditor(list).Get(), 
				index, 
				type, 
				dataPtr, 
				dataSize
			)
		);
	}
	
	void AEPutDesc(Owned<AEDescList>& list, long index, const AEDesc& desc) 
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutDesc(
				&AEDescEditor(list).Get(), 
				index, 
				&desc
			)
		);
	}
	
	GetNthPtr_Result AEGetNthPtr(
		const AEDesc& listDesc, 
		long index, 
		DescType desiredType, 
		void* dataPtr, 
		Size maximumSize) 
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		GetNthPtr_Result result;
		
		ThrowOSStatus(
			::AEGetNthPtr(
				&listDesc, 
				index, 
				desiredType, 
				&result.keyword, 
				&result.typeCode, 
				dataPtr, 
				maximumSize, 
				&result.actualSize
			)
		);
		return result;
	}
	
	Owned<AEDesc> AEGetNthDesc(
		const AEDesc& listDesc, 
		long index, 
		DescType desiredType, 
		::AEKeyword* keywordResult
	)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
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
		return Owned<AEDesc>::Seize(result);
	}
	
	Owned<AEDesc> AEGetNthDesc(
		const AEDesc& listDesc, 
		long index, 
		::AEKeyword* keywordResult
	)
	{
		return AEGetNthDesc(
			listDesc, 
			index, 
			typeWildCard, 
			keywordResult
		);
	}
	
	Size AESizeOfNthItem(const AEDescList& list, long index)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEGetParamPtr_Result result;
		
		ThrowOSStatus(
			::AESizeOfNthItem(
				&list, 
				index, 
				&result.typeCode, 
				&result.dataSize
			)
		);
		
		return result;
	}
	
	void AEDeleteItem( AEDesc& listDesc, long index )
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus( ::AEDeleteItem(&listDesc, index) );
	}
	
	void AEPutKeyPtr(
		Owned<AERecord>& record, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutKeyPtr(
				&AEDescEditor(record).Get(), 
				keyword, 
				typeCode, 
				dataPtr, 
				dataSize
			)
		);
	}
	
	void AEPutKeyDesc(
		Owned<AERecord>& record, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutKeyDesc(
				&AEDescEditor(record).Get(), 
				keyword, 
				&desc
			)
		);
	}
	
	AEGetKeyPtr_Result AEGetKeyPtr(
		const AERecord& record, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEGetKeyPtr_Result result;
		
		ThrowOSStatus(
			::AEGetKeyPtr(
				&record, 
				keyword, 
				desiredType, 
				&result.typeCode, 
				dataPtr, 
				maximumSize, 
				&result.dataSize
			)
		);
		return result;
	}
	
	Owned<AEDesc> AEGetKeyDesc(
		const AERecord& record, 
		AEKeyword keyword, 
		DescType desiredType)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc result;
		ThrowOSStatus(
			::AEGetKeyDesc(
				&record, 
				keyword, 
				desiredType, 
				&result
			)
		);
		return Owned<AEDesc>::Seize( result );
	}
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc(
		const AERecord& record, 
		AEKeyword keyword)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AESizeOfKeyDesc_Result result;
		ThrowOSStatus(
			::AESizeOfKeyDesc(
				&record, 
				keyword, 
				&result.typeCode, 
				&result.dataSize
			)
		);
		return result;
	}
	
	void AEDeleteKeyDesc(
		Owned<AERecord>& record, 
		AEKeyword keyword)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEDeleteKeyDesc(
				&AEDescEditor(record).Get(), 
				keyword
			)
		);
	}
	
	Owned<AppleEvent> AECreateAppleEvent(
		AEEventClass eventClass, 
		AEEventID eventID, 
		const AEAddressDesc& target, 
		AEReturnID returnID, 
		AETransactionID transactionID)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
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
		return Owned<AppleEvent>::Seize( result );
	}
	
	void AEPutParamPtr(
		Owned<AppleEvent>& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutParamPtr(
				&AEDescEditor(appleEvent).Get(), 
				keyword, 
				typeCode, 
				dataPtr, 
				dataSize
			)
		);
	}
	
	void AEPutParamDesc(
		Owned<AppleEvent>& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutParamDesc(
				&AEDescEditor(appleEvent).Get(), 
				keyword, 
				&desc
			)
		);
	}
	
	AEGetParamPtr_Result AEGetParamPtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEGetParamPtr_Result result;
		
		ThrowOSStatus(
			::AEGetParamPtr(
				&appleEvent, 
				keyword, 
				desiredType, 
				&result.typeCode, 
				dataPtr, 
				maximumSize, 
				&result.dataSize
			)
		);
		return result;
	}
	
	Owned<AEDesc> AEGetParamDesc(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc result;
		ThrowOSStatus(
			::AEGetParamDesc(
				&appleEvent, 
				keyword, 
				desiredType, 
				&result
			)
		);
		return Owned<AEDesc>::Seize( result );
	}
	
	AESizeOfParam_Result AESizeOfParam(
		const AppleEvent& appleEvent, 
		AEKeyword keyword)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AESizeOfParam_Result result;
		ThrowOSStatus(
			::AESizeOfParam(
				&appleEvent, 
				keyword, 
				&result.typeCode, 
				&result.dataSize
			)
		);
		return result;
	}
	
	void AEDeleteParam(
		Owned<AppleEvent>& appleEvent, 
		AEKeyword keyword)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEDeleteParam(
				&AEDescEditor(appleEvent).Get(), 
				keyword
			)
		);
	}
	
	void AEPutAttributePtr(
		Owned<AppleEvent>& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutAttributePtr(
				&AEDescEditor(appleEvent).Get(), 
				keyword, 
				typeCode, 
				dataPtr, 
				dataSize
			)
		);
	}
	
	void AEPutAttributeDesc(
		Owned<AppleEvent>& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEPutAttributeDesc(
				&AEDescEditor(appleEvent).Get(), 
				keyword, 
				&desc
			)
		);
	}
	
	AEGetAttributePtr_Result AEGetAttributePtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEGetAttributePtr_Result result;
		
		ThrowOSStatus(
			::AEGetAttributePtr(
				&appleEvent, 
				keyword, 
				desiredType, 
				&result.typeCode, 
				dataPtr, 
				maximumSize, 
				&result.dataSize
			)
		);
		return result;
	}
	
	Owned<AEDesc> AEGetAttributeDesc(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AEDesc result;
		ThrowOSStatus(
			::AEGetAttributeDesc(
				&appleEvent, 
				keyword, 
				desiredType, 
				&result
			)
		);
		return Owned<AEDesc>::Seize( result );
	}
	
	AESizeOfAttribute_Result AESizeOfAttribute(
		const AppleEvent& appleEvent, 
		AEKeyword keyword)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		AESizeOfAttribute_Result result;
		ThrowOSStatus(
			::AESizeOfAttribute(
				&appleEvent, 
				keyword, 
				&result.typeCode, 
				&result.dataSize
			)
		);
		return result;
	}
	
	void AEGetDescData( const AEDesc& desc, void* dataPtr, std::size_t maximumSize )
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus( ::AEGetDescData( &desc, dataPtr, maximumSize ) );
	}
	
	void AEReplaceDescData(
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize, 
		AEDesc& result)
	{
		OnlyOnce<RegisterAppleEventManagerErrors>();
		ThrowOSStatus(
			::AEReplaceDescData(
				typeCode, 
				dataPtr, 
				dataSize,  
				&result
			)
		);
	}
	
}

