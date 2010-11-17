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
	
	using ::Nitrogen::AEDesc_Data;
	using ::Nitrogen::AEDesc_Token;
	
	static const AEKeyDesc global_null_AEKeyDesc = { 0, { typeNull, NULL } };
	
	const AEDesc_Data&  null_resource< AEDesc_Data  >::value = (AEDesc_Data&)  global_null_AEKeyDesc.descContent;
	const AEDesc_Token& null_resource< AEDesc_Token >::value = (AEDesc_Token&) global_null_AEKeyDesc.descContent;
	
	const AEKeyDesc&    null_resource< AEKeyDesc    >::value = global_null_AEKeyDesc;
	
}

namespace Nitrogen
{
	
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
	
	
	AECoercionHandler::AECoercionHandler() : fromType(),
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
	
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall )
	{
		ThrowOSStatus( ::AEInstallCoercionHandler( toInstall.fromType, 
		                                           toInstall.toType, 
		                                           toInstall.handler, 
		                                           toInstall.handlerRefCon, 
		                                           toInstall.fromTypeIsDesc,
		                                           toInstall.isSysHandler ) );
		
		return nucleus::owned< AECoercionHandler >::seize( toInstall );
	}
	
	AECoercionHandler AEGetCoercionHandler( DescType  fromType,
	                                        DescType  toType,
	                                        bool      isSysHandler )
	{
		::AECoercionHandlerUPP  handler;
		long                    handlerRefCon;
		::Boolean               fromTypeIsDesc;
		
		ThrowOSStatus( ::AEGetCoercionHandler( fromType,
		                                       toType,
		                                       &handler,
		                                       &handlerRefCon,
		                                       &fromTypeIsDesc,
		                                       isSysHandler ) );
		
		return AECoercionHandler( fromType,
		                          toType,
		                          handler,
		                          handlerRefCon,
		                          fromTypeIsDesc,
		                          isSysHandler );
	}
	
	nucleus::owned< AEDesc_Data > AECoercePtr( DescType     typeCode, 
	                                           const void*  dataPtr, 
	                                           Size         dataSize, 
	                                           DescType     toType )
	{
		// Necessary for OS 9; OS X does this automatically
		if ( toType == typeWildCard )
		{
			toType = typeCode;
		}
		
		AEDesc_Data result;
		
		ThrowOSStatus( ::AECoercePtr( typeCode,
		                              dataPtr,
		                              dataSize,
		                              toType,
		                              &result ) );
		
		return nucleus::owned< AEDesc_Data >::seize( result );
	}
	
	nucleus::owned< AEDesc_Data > AECoerceDesc( const AEDesc& desc, DescType toType )
	{
		// Necessary for OS 9; OS X does this automatically
		if ( toType == typeWildCard )
		{
			toType = DescType( desc.descriptorType );
		}
		
		AEDesc_Data result;
		
		ThrowOSStatus( ::AECoerceDesc( &desc, toType, &result ) );
		
		return nucleus::owned< AEDesc_Data >::seize( result );
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
		
		AEDesc AECreateDesc_Unowned( DescType     typeCode,
		                             const void*  dataPtr,
		                             Size         dataSize )
		{
			if ( dataPtr == NULL )
			{
				throw ParamErr();  // should assert?
			}
			
			AEDesc desc;
			
			ThrowOSStatus( ::AECreateDesc( typeCode, dataPtr, dataSize, &desc ) );
			
			return desc;
		}
		
		AEDesc AECreateDesc_Unowned( DescType typeCode, Handle handle )
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
		
		AEDesc AECreateDesc_Unowned( DescType typeCode, nucleus::owned< Handle > handle )
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
	                                 DescType       desiredType,
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
	                                 AEKeyword        keyword,
	                                 DescType         desiredType )
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
	
	void AEPutPtr( AEDescList&  list, 
	               long         index, 
	               DescType     type, 
	               const void*  dataPtr, 
	               Size         dataSize )
	{
		ThrowOSStatus( ::AEPutPtr( &list, index, type, dataPtr, dataSize ) );
	}
	
	void AEPutDesc( AEDescList& list, long index, const AEDesc& desc )
	{
		ThrowOSStatus( ::AEPutDesc( &list, index, &desc ) );
	}
	
	GetNthPtr_Result AEGetNthPtr( const AEDesc&  listDesc,
	                              long           index,
	                              DescType       desiredType,
	                              void*          dataPtr,
	                              Size           maximumSize )
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
		
		result.keyword    = AEKeyword( keyword );
		result.typeCode   = DescType( typeCode );
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
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteItem( AEDescList_Data& list, long index )
	{
		ThrowOSStatus( ::AEDeleteItem( &list, index ) );
	}
	
	void AEDeleteItem( nucleus::owned< AEDescList_Data >& list, long index )
	{
		AEDeleteItem( Detail::AEDescEditor< AEDescList_Data >( list ), index );
	}
	
	void AEPutKeyPtr( AERecord&    record,
	                  AEKeyword    keyword,
	                  DescType     typeCode,
	                  const void*  dataPtr,
	                  std::size_t  dataSize )
	{
		nucleus::scoped< ::DescType > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEPutKeyPtr( &record,
		                              keyword,
		                              typeCode,
		                              dataPtr,
		                              dataSize ) );
	}
	
	void AEPutKeyDesc( AERecord&      record,
	                   AEKeyword      keyword,
	                   const AEDesc&  desc )
	{
		nucleus::scoped< ::DescType > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEPutKeyDesc( &record, keyword, &desc ) );
	}
	
	void AEPutKeyDesc( AERecord&         record,
	                   const AEKeyDesc&  keyDesc )
	{
		AEPutKeyDesc( record,
		              AEKeyword( keyDesc.descKey ),
		              keyDesc.descContent );
	}
	
	AEGetKeyPtr_Result AEGetKeyPtr( const AERecord&  record,
	                                AEKeyword        keyword,
	                                DescType         desiredType,
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
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc( const AERecord&  record, 
	                                        AEKeyword        keyword )
	{
		AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
		
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AESizeOfKeyDesc( &fixedAERecord,
		                                  keyword,
		                                  &typeCode,
		                                  &dataSize ) );
		
		AESizeOfKeyDesc_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteKeyDesc( AERecord& record, AEKeyword keyword )
	{
		nucleus::scoped< ::DescType > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEDeleteKeyDesc( &record, keyword ) );
	}
	
	nucleus::owned< AppleEvent > AECreateAppleEvent( AEEventClass          eventClass,
	                                                 AEEventID             eventID,
	                                                 const AEAddressDesc&  target,
	                                                 AEReturnID            returnID,
	                                                 AETransactionID       transactionID )
	{
		AppleEvent result;
		
		ThrowOSStatus( ::AECreateAppleEvent( eventClass,
		                                     eventID,
		                                     &target,
		                                     returnID,
		                                     transactionID,
		                                     &result ) );
		
		return nucleus::owned< AppleEvent >::seize( result );
	}
	
	void AEPutParamPtr( AppleEvent&  appleEvent,
	                    AEKeyword    keyword,
	                    DescType     typeCode,
	                    const void*  dataPtr,
	                    std::size_t  dataSize )
	{
		ThrowOSStatus( ::AEPutParamPtr( &appleEvent,
		                                keyword,
		                                typeCode,
		                                dataPtr,
		                                dataSize ) );
	}
	
	void AEPutParamPtr( nucleus::owned< AppleEvent >&  appleEvent,
	                    AEKeyword                      keyword,
	                    DescType                       typeCode,
	                    const void*                    dataPtr,
	                    std::size_t                    dataSize )
	{
		AEPutParamPtr( Detail::AEDescEditor< AppleEvent >( appleEvent ),
		               keyword,
		               typeCode,
		               dataPtr,
		               dataSize );
	}
	
	void AEPutParamDesc( AppleEvent&    appleEvent,
	                     AEKeyword      keyword,
	                     const AEDesc&  desc )
	{
		ThrowOSStatus( ::AEPutParamDesc( &appleEvent, keyword, &desc ) );
	}
	
	void AEPutParamDesc( nucleus::owned< AppleEvent >&  appleEvent,
	                     AEKeyword                      keyword,
	                     const AEDesc&                  desc )
	{
		AEPutParamDesc( Detail::AEDescEditor< AppleEvent >( appleEvent ), keyword, desc );
	}
	
	AEGetParamPtr_Result AEGetParamPtr( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword,
	                                    DescType           desiredType,
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
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	nucleus::owned< AEDesc_Data > AEGetParamDesc( const AppleEvent&  appleEvent,
	                                              AEKeyword          keyword,
	                                              DescType           desiredType )
	{
		AEDesc_Data result;
		
		ThrowOSStatus( ::AEGetParamDesc( &appleEvent,
		                                 keyword,
		                                 desiredType,
		                                 &result ) );
		
		return nucleus::owned< AEDesc_Data >::seize( result );
	}
	
	AESizeOfParam_Result AESizeOfParam( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword )
	{
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AESizeOfParam( &appleEvent,
		                                keyword,
		                                &typeCode,
		                                &dataSize ) );
		
		AESizeOfParam_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteParam( AppleEvent& appleEvent, AEKeyword keyword )
	{
		ThrowOSStatus( ::AEDeleteParam( &appleEvent, keyword ) );
	}
	
	void AEDeleteParam( nucleus::owned< AppleEvent >&  appleEvent,
	                    AEKeyword                      keyword )
	{
		AEDeleteParam( Detail::AEDescEditor< AppleEvent >( appleEvent ), keyword );
	}
	
	void AEPutAttributePtr( AppleEvent&  appleEvent,
	                        AEKeyword    keyword,
	                        DescType     typeCode,
	                        const void*  dataPtr,
	                        std::size_t  dataSize )
	{
		ThrowOSStatus( ::AEPutAttributePtr( &appleEvent,
		                                    keyword,
		                                    typeCode,
		                                    dataPtr,
		                                    dataSize ) );
	}
	
	void AEPutAttributePtr( nucleus::owned< AppleEvent >&  appleEvent,
	                        AEKeyword                      keyword,
	                        DescType                       typeCode,
	                        const void*                    dataPtr,
	                        std::size_t                    dataSize )
	{
		AEPutAttributePtr( Detail::AEDescEditor< AppleEvent >( appleEvent ),
		                   keyword,
		                   typeCode,
		                   dataPtr,
		                   dataSize );
	}
	
	void AEPutAttributeDesc( AppleEvent&    appleEvent,
	                         AEKeyword      keyword,
	                         const AEDesc&  desc )
	{
		ThrowOSStatus( ::AEPutAttributeDesc( &appleEvent, keyword, &desc ) );
	}
	
	void AEPutAttributeDesc( nucleus::owned< AppleEvent >&  appleEvent,
	                         AEKeyword                      keyword,
	                         const AEDesc&                  desc )
	{
		AEPutAttributeDesc( Detail::AEDescEditor< AppleEvent >( appleEvent ), keyword, desc );
	}
	
	AEGetAttributePtr_Result AEGetAttributePtr( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword,
	                                            DescType           desiredType,
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
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	nucleus::owned< AEDesc_Data > AEGetAttributeDesc( const AppleEvent&  appleEvent,
	                                                  AEKeyword          keyword,
	                                                  DescType           desiredType )
	{
		AEDesc_Data result;
		
		ThrowOSStatus( ::AEGetAttributeDesc( &appleEvent,
		                                     keyword,
		                                     desiredType,
		                                     &result ) );
		
		return nucleus::owned< AEDesc_Data >::seize( result );
	}
	
	AESizeOfAttribute_Result AESizeOfAttribute( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword )
	{
		::DescType typeCode;
		::Size dataSize;
		
		ThrowOSStatus( ::AESizeOfAttribute( &appleEvent,
		                                    keyword,
		                                    &typeCode,
		                                    &dataSize ) );
		
		AESizeOfAttribute_Result result;
		
		result.typeCode = DescType( typeCode );
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEGetDescData( const AEDesc& desc, void* dataPtr, std::size_t maximumSize )
	{
		ThrowOSStatus( ::AEGetDescData( &desc, dataPtr, maximumSize ) );
	}
	
	void AEReplaceDescData( DescType     typeCode,
	                        const void*  dataPtr,
	                        std::size_t  dataSize,
	                        AEDesc&      result )
	{
		ThrowOSStatus( ::AEReplaceDescData( typeCode, dataPtr, dataSize, &result ) );
	}
	
	void AEReplaceDescData( DescType                        typeCode,
	                        const void*                     dataPtr,
	                        std::size_t                     dataSize,
	                        nucleus::owned< AEDesc_Data >&  result)
	{
		AEReplaceDescData( typeCode, dataPtr, dataSize, Detail::AEDescEditor< AEDesc_Data >( result ) );
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
	
	
	namespace CompileTests
	{
		
		namespace
		{
			
			pascal OSErr AECoercerOfDescs_ProcPtr( const AEDesc*  /*fromDesc*/,
			                                       ::DescType     /*toType*/,
			                                       long           /*refCon*/,
			                                       AEDesc*        /*result*/ )
			{
				return errAEEventNotHandled;
			}
			
			nucleus::owned< AEDesc_Data > AECoercerOfDescs_N( const AEDesc_Data&  fromDesc,
			                                                  DescType            /*toType*/,
			                                                  RefCon              /*refCon*/ )
			{
				return nucleus::owned< AEDesc_Data >::seize( fromDesc );
			}
			
		}
		
		static nucleus::owned< AECoercionHandler > InstallCoercionHandler_ProcPtr()
		{
			return AEInstallCoercionHandler< AECoercerOfDescs_ProcPtr >( DescType( 'from' ), DescType( 'to  ' ) );
		}
		
		static nucleus::owned< AECoercionHandler > InstallCoercionHandler_N()
		{
			return AEInstallCoercionHandler< AECoercerOfDescs_N >( DescType( 'from' ), DescType( 'to  ' ) );
		}
		
		static nucleus::string GetTextAttr( const AppleEvent& appleEvent, AEKeyword keyword )
		{
			return AEGetAttributePtr< typeChar >( appleEvent, keyword );
		}
		
		static AEReturnID_32Bit GetReturnID( const AppleEvent& appleEvent )
		{
			return AEGetAttributePtr< keyReturnIDAttr >( appleEvent );
		}
		
		static AEEventClass GetEventClass( const AppleEvent& appleEvent )
		{
			return AEGetAttributePtr< keyEventClassAttr >( appleEvent );
		}
		
	}
	
}

