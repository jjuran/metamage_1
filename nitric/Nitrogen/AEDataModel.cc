// Nitrogen/AEDataModel.cc
// -----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/AEDataModel.hh"

// nucleus
#include "nucleus/scoped.hh"

// Nitrogen
#include "Nitrogen/MacErrors.hh"


namespace nucleus
{
	
	using ::Mac::AEDesc_Data;
	using ::Mac::AEDesc_Token;
	
	static const AEKeyDesc global_null_AEKeyDesc = { 0, { typeNull, NULL } };
	
	const AEDesc_Data&  null_resource< AEDesc_Data  >::value = (AEDesc_Data&)  global_null_AEKeyDesc.descContent;
	const AEDesc_Token& null_resource< AEDesc_Token >::value = (AEDesc_Token&) global_null_AEKeyDesc.descContent;
	
	const AEKeyDesc&    null_resource< AEKeyDesc    >::value = global_null_AEKeyDesc;
	
}

namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( AppleEventManager )
	
	
	static void RegisterAppleEventManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class AppleEventManagerErrorsRegistration
	{
		public:
			AppleEventManagerErrorsRegistration()  { RegisterAppleEventManagerErrors(); }
	};
	
	static AppleEventManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	nucleus::owned< Mac::AEDesc_Data > AECoercePtr( Mac::DescType  typeCode,
	                                                const void*    dataPtr,
	                                                std::size_t    dataSize,
	                                                Mac::DescType  toType )
	{
		// Necessary for OS 9; OS X does this automatically
		if ( toType == Mac::typeWildCard )
		{
			toType = typeCode;
		}
		
		Mac::AEDesc_Data result;
		
		ThrowOSStatus( ::AECoercePtr( typeCode,
		                              dataPtr,
		                              dataSize,
		                              toType,
		                              &result ) );
		
		return nucleus::owned< Mac::AEDesc_Data >::seize( result );
	}
	
	nucleus::owned< Mac::AEDesc_Data > AECoerceDesc( const AEDesc& desc, Mac::DescType toType )
	{
		// Necessary for OS 9; OS X does this automatically
		if ( toType == Mac::typeWildCard )
		{
			toType = Mac::DescType( desc.descriptorType );
		}
		
		Mac::AEDesc_Data result;
		
		ThrowOSStatus( ::AECoerceDesc( &desc, toType, &result ) );
		
		return nucleus::owned< Mac::AEDesc_Data >::seize( result );
	}
	
	// Mac OS X allows AERecord arguments to have any descriptorType.
	// Mac OS 9 does not, so we fake this behavior by adjusting the descriptorType
	// prior to calling the function.
	// We need two ways of doing this -- one for const AEDesc& and one for non-const.
	// For const (AEGetKey{Ptr,Desc}, AESizeOfKeyDesc), we call Fix_AERecordDescriptorType
	// to return an adjusted AERecord, a pointer to which is passed to the function.
	// For non-const (AEPutKey{Ptr,Desc}, AEDeleteKeyDesc), we use scoped< ::DescType >,
	// which adjusts the actual AEDesc argument that was passed, and afterward adjusts it back.
	
	static AERecord Fix_AERecordDescriptorType( AERecord record )
	{
		record.descriptorType = typeAERecord;
		
		return record;
	}
	
	namespace Detail
	{
		
		AEDesc AECreateDesc_Unowned( Mac::DescType  typeCode,
		                             const void*    dataPtr,
		                             std::size_t    dataSize )
		{
			if ( dataPtr == NULL )
			{
				throw ParamErr();  // should assert?
			}
			
			AEDesc desc;
			
			ThrowOSStatus( ::AECreateDesc( typeCode, dataPtr, dataSize, &desc ) );
			
			return desc;
		}
		
		AEDesc AECreateDesc_Unowned( Mac::DescType typeCode, Handle handle )
		{
			if ( handle.Get() == NULL )
			{
				throw ParamErr();  // should assert?
			}
			
		#if !TARGET_API_MAC_OSX
			
			nucleus::saved< Handle_State > hState( Handle_State( handle ) );
			
			HLock( handle );
			
		#endif
			
			return AECreateDesc_Unowned( typeCode, *handle, GetHandleSize( handle ) );
		}
		
		AEDesc AECreateDesc_Unowned( Mac::DescType typeCode, nucleus::owned< Handle > handle )
		{
		#if OPAQUE_TOOLBOX_STRUCTS
			
			return AECreateDesc_Unowned( typeCode, handle.get() );
			
		#else
			
			AEDesc desc;
			
			desc.descriptorType = typeCode;
			desc.dataHandle     = handle.release();
			
			return desc;
			
		#endif
		}
		
		AEDesc AEDuplicateDesc_Unowned( const AEDesc& desc )
		{
			AEDesc result;
			
			ThrowOSStatus( ::AEDuplicateDesc( &desc, &result ) );
			
			return result;
		}
		
		AEDesc AECreateList_Unowned( const void*  factoringPtr,
	                                 std::size_t  factoredSize,
	                                 bool         isRecord )
		{
			AEDesc desc;
			
			ThrowOSStatus( ::AECreateList( factoringPtr, factoredSize, isRecord, &desc ) );
			
			return desc;
		}
		
		AEDesc AEGetNthDesc_Unowned( const AEDesc&  listDesc,
	                                 long           index,
	                                 Mac::DescType  desiredType,
	                                 ::AEKeyword*   keywordResult )
		{
			::AEKeyword keyword;
			AEDesc result;
			
			ThrowOSStatus( ::AEGetNthDesc( &listDesc, 
			                               index, 
			                               desiredType, 
			                               keywordResult != NULL ? keywordResult
			                                                     : &keyword, 
			                               &result ) );
			
			return result;
		}
		
		AEDesc AEGetKeyDesc_Unowned( const AERecord&  record,
	                                 Mac::AEKeyword   keyword,
	                                 Mac::DescType    desiredType )
		{
			AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
			
			AEDesc result;
			
			ThrowOSStatus( ::AEGetKeyDesc( &fixedAERecord,
			                               keyword,
			                               desiredType,
			                               &result ) );
			
			return result;
		}
		
	}
	
	long AECountItems( const AEDesc& desc )
	{
		long count;
		
		ThrowOSStatus( ::AECountItems( &desc, &count ) );
		
		return count;
	}
	
	void AEPutPtr( AEDescList&    list, 
	               long           index, 
	               Mac::DescType  type, 
	               const void*    dataPtr, 
	               std::size_t    dataSize )
	{
		ThrowOSStatus( ::AEPutPtr( &list, index, type, dataPtr, dataSize ) );
	}
	
	void AEPutDesc( AEDescList& list, long index, const AEDesc& desc )
	{
		ThrowOSStatus( ::AEPutDesc( &list, index, &desc ) );
	}
	
	GetNthPtr_Result AEGetNthPtr( const AEDesc&  listDesc,
	                              long           index,
	                              Mac::DescType  desiredType,
	                              void*          dataPtr,
	                              std::size_t    maximumSize )
	{
		::AEKeyword keyword;
		::DescType  typeCode;
		::Size      actualSize;
		
		ThrowOSStatus( ::AEGetNthPtr( &listDesc, 
		                              index, 
		                              desiredType, 
		                              &keyword, 
		                              &typeCode, 
		                              dataPtr, 
		                              maximumSize, 
		                              &actualSize ) );
		
		GetNthPtr_Result result;
		
		result.keyword    = Mac::AEKeyword( keyword );
		result.typeCode   = Mac::DescType( typeCode );
		result.actualSize = actualSize;
		
		return result;
	}
	
	AESizeOfNthItem_Result AESizeOfNthItem( const AEDescList& list, long index )
	{
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AESizeOfNthItem( &list, 
		                                  index, 
		                                  &typeCode, 
		                                  &dataSize ) );
		
		AESizeOfNthItem_Result result;
		
		result.typeCode = Mac::DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteItem( Mac::AEDescList_Data& list, long index )
	{
		ThrowOSStatus( ::AEDeleteItem( &list, index ) );
	}
	
	void AEDeleteItem( nucleus::owned< Mac::AEDescList_Data >& list, long index )
	{
		AEDeleteItem( Detail::mutable_AEDesc( list ), index );
	}
	
	void AEPutKeyPtr( AERecord&       record,
	                  Mac::AEKeyword  keyword,
	                  Mac::DescType   typeCode,
	                  const void*     dataPtr,
	                  std::size_t     dataSize )
	{
		nucleus::scoped< ::DescType > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEPutKeyPtr( &record,
		                              keyword,
		                              typeCode,
		                              dataPtr,
		                              dataSize ) );
	}
	
	void AEPutKeyDesc( AERecord&       record,
	                   Mac::AEKeyword  keyword,
	                   const AEDesc&   desc )
	{
		nucleus::scoped< ::DescType > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEPutKeyDesc( &record, keyword, &desc ) );
	}
	
	void AEPutKeyDesc( AERecord&         record,
	                   const AEKeyDesc&  keyDesc )
	{
		AEPutKeyDesc( record,
		              Mac::AEKeyword( keyDesc.descKey ),
		              keyDesc.descContent );
	}
	
	AEGetKeyPtr_Result AEGetKeyPtr( const AERecord&  record,
	                                Mac::AEKeyword   keyword,
	                                Mac::DescType    desiredType,
	                                void*            dataPtr,
	                                std::size_t      maximumSize )
	{
		AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AEGetKeyPtr( &fixedAERecord,
		                              keyword,
		                              desiredType,
		                              &typeCode,
		                              dataPtr,
		                              maximumSize,
		                              &dataSize ) );
		
		AEGetKeyPtr_Result result;
		
		result.typeCode = Mac::DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc( const AERecord&  record, 
	                                        Mac::AEKeyword   keyword )
	{
		AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AESizeOfKeyDesc( &fixedAERecord,
		                                  keyword,
		                                  &typeCode,
		                                  &dataSize ) );
		
		AESizeOfKeyDesc_Result result;
		
		result.typeCode = Mac::DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteKeyDesc( AERecord& record, Mac::AEKeyword keyword )
	{
		nucleus::scoped< ::DescType > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEDeleteKeyDesc( &record, keyword ) );
	}
	
	nucleus::owned< Mac::AppleEvent >
	//
	AECreateAppleEvent( Mac::AEEventClass          eventClass,
	                    Mac::AEEventID             eventID,
	                    const Mac::AEAddressDesc&  target,
	                    Mac::AEReturnID            returnID,
	                    Mac::AETransactionID       transactionID )
	{
		Mac::AppleEvent result;
		
		ThrowOSStatus( ::AECreateAppleEvent( eventClass,
		                                     eventID,
		                                     &target,
		                                     returnID,
		                                     transactionID,
		                                     &result ) );
		
		return nucleus::owned< Mac::AppleEvent >::seize( result );
	}
	
	void AEPutParamPtr( AppleEvent&     appleEvent,
	                    Mac::AEKeyword  keyword,
	                    Mac::DescType   typeCode,
	                    const void*     dataPtr,
	                    std::size_t     dataSize )
	{
		ThrowOSStatus( ::AEPutParamPtr( &appleEvent,
		                                keyword,
		                                typeCode,
		                                dataPtr,
		                                dataSize ) );
	}
	
	void AEPutParamPtr( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                    Mac::AEKeyword                      keyword,
	                    Mac::DescType                       typeCode,
	                    const void*                         dataPtr,
	                    std::size_t                         dataSize )
	{
		AEPutParamPtr( Detail::mutable_AEDesc( appleEvent ),
		               keyword,
		               typeCode,
		               dataPtr,
		               dataSize );
	}
	
	void AEPutParamDesc( AppleEvent&     appleEvent,
	                     Mac::AEKeyword  keyword,
	                     const AEDesc&   desc )
	{
		ThrowOSStatus( ::AEPutParamDesc( &appleEvent, keyword, &desc ) );
	}
	
	void AEPutParamDesc( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                     Mac::AEKeyword                      keyword,
	                     const AEDesc&                       desc )
	{
		AEPutParamDesc( Detail::mutable_AEDesc( appleEvent ), keyword, desc );
	}
	
	AEGetParamPtr_Result AEGetParamPtr( const AppleEvent&  appleEvent,
	                                    Mac::AEKeyword     keyword,
	                                    Mac::DescType      desiredType,
	                                    void*              dataPtr,
	                                    std::size_t        maximumSize )
	{
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AEGetParamPtr( &appleEvent,
		                                keyword,
		                                desiredType,
		                                &typeCode,
		                                dataPtr,
		                                maximumSize,
		                                &dataSize ) );
		
		AEGetParamPtr_Result result;
		
		result.typeCode = Mac::DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	AEGetParamDesc( const AppleEvent&  appleEvent,
	                Mac::AEKeyword     keyword,
	                Mac::DescType      desiredType )
	{
		Mac::AEDesc_Data result;
		
		ThrowOSStatus( ::AEGetParamDesc( &appleEvent,
		                                 keyword,
		                                 desiredType,
		                                 &result ) );
		
		return nucleus::owned< Mac::AEDesc_Data >::seize( result );
	}
	
	AESizeOfParam_Result AESizeOfParam( const AppleEvent&  appleEvent,
	                                    Mac::AEKeyword     keyword )
	{
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AESizeOfParam( &appleEvent,
		                                keyword,
		                                &typeCode,
		                                &dataSize ) );
		
		AESizeOfParam_Result result;
		
		result.typeCode = Mac::DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteParam( AppleEvent& appleEvent, Mac::AEKeyword keyword )
	{
		ThrowOSStatus( ::AEDeleteParam( &appleEvent, keyword ) );
	}
	
	void AEDeleteParam( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                    Mac::AEKeyword                      keyword )
	{
		AEDeleteParam( Detail::mutable_AEDesc( appleEvent ), keyword );
	}
	
	void AEPutAttributePtr( AppleEvent&     appleEvent,
	                        Mac::AEKeyword  keyword,
	                        Mac::DescType   typeCode,
	                        const void*     dataPtr,
	                        std::size_t     dataSize )
	{
		ThrowOSStatus( ::AEPutAttributePtr( &appleEvent,
		                                    keyword,
		                                    typeCode,
		                                    dataPtr,
		                                    dataSize ) );
	}
	
	void AEPutAttributePtr( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                        Mac::AEKeyword                      keyword,
	                        Mac::DescType                       typeCode,
	                        const void*                         dataPtr,
	                        std::size_t                         dataSize )
	{
		AEPutAttributePtr( Detail::mutable_AEDesc( appleEvent ),
		                   keyword,
		                   typeCode,
		                   dataPtr,
		                   dataSize );
	}
	
	void AEPutAttributeDesc( AppleEvent&     appleEvent,
	                         Mac::AEKeyword  keyword,
	                         const AEDesc&   desc )
	{
		ThrowOSStatus( ::AEPutAttributeDesc( &appleEvent, keyword, &desc ) );
	}
	
	void AEPutAttributeDesc( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                         Mac::AEKeyword                      keyword,
	                         const AEDesc&                       desc )
	{
		AEPutAttributeDesc( Detail::mutable_AEDesc( appleEvent ), keyword, desc );
	}
	
	AEGetAttributePtr_Result AEGetAttributePtr( const AppleEvent&  appleEvent,
	                                            Mac::AEKeyword     keyword,
	                                            Mac::DescType      desiredType,
	                                            void*              dataPtr,
	                                            std::size_t        maximumSize )
	{
		::DescType typeCode;
		::Size     dataSize;
		
		ThrowOSStatus( ::AEGetAttributePtr( &appleEvent,
		                                    keyword,
		                                    desiredType,
		                                    &typeCode,
		                                    dataPtr,
		                                    maximumSize,
		                                    &dataSize ) );
		
		AEGetAttributePtr_Result result;
		
		result.typeCode = Mac::DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	AEGetAttributeDesc( const AppleEvent&  appleEvent,
	                    Mac::AEKeyword     keyword,
	                    Mac::DescType      desiredType )
	{
		Mac::AEDesc_Data result;
		
		ThrowOSStatus( ::AEGetAttributeDesc( &appleEvent,
		                                     keyword,
		                                     desiredType,
		                                     &result ) );
		
		return nucleus::owned< Mac::AEDesc_Data >::seize( result );
	}
	
	AESizeOfAttribute_Result AESizeOfAttribute( const AppleEvent&  appleEvent,
	                                            Mac::AEKeyword     keyword )
	{
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AESizeOfAttribute( &appleEvent,
		                                    keyword,
		                                    &typeCode,
		                                    &dataSize ) );
		
		AESizeOfAttribute_Result result;
		
		result.typeCode = Mac::DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEGetDescData( const AEDesc& desc, void* dataPtr, std::size_t maximumSize )
	{
		ThrowOSStatus( ::AEGetDescData( &desc, dataPtr, maximumSize ) );
	}
	
	void AEReplaceDescData( Mac::DescType  typeCode,
	                        const void*    dataPtr,
	                        std::size_t    dataSize,
	                        AEDesc&        result )
	{
		ThrowOSStatus( ::AEReplaceDescData( typeCode, dataPtr, dataSize, &result ) );
	}
	
	void AEReplaceDescData( Mac::DescType                        typeCode,
	                        const void*                          dataPtr,
	                        std::size_t                          dataSize,
	                        nucleus::owned< Mac::AEDesc_Data >&  result )
	{
		AEReplaceDescData( typeCode,
		                   dataPtr,
		                   dataSize,
		                   Detail::mutable_AEDesc( result ) );
	}
	
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
	
#if defined( __GNUC__ ) && ( __GNUC__ * 100 + __GNUC_MINOR__ <= 303 )
	
	// Panther's g++ 3.3 can't handle this.
	
#else
	
	namespace CompileTests
	{
		
		static nucleus::string GetTextAttr( const Mac::AppleEvent& appleEvent, Mac::AEKeyword keyword )
		{
			return AEGetAttributePtr< Mac::typeChar >( appleEvent, keyword );
		}
		
		static AEReturnID_32Bit GetReturnID( const Mac::AppleEvent& appleEvent )
		{
			return AEGetAttributePtr< Mac::keyReturnIDAttr >( appleEvent );
		}
		
		static AEEventClass GetEventClass( const Mac::AppleEvent& appleEvent )
		{
			return AEGetAttributePtr< Mac::keyEventClassAttr >( appleEvent );
		}
		
	}
	
#endif  // #if defined( __GNUC__ ) && ( __GNUC__ * 100 + __GNUC_MINOR__ <= 303 )
	
}
