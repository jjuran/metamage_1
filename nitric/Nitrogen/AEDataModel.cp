// AEDataModel.cp

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif

#ifndef NUCLEUS_SCOPED_H
#include "Nucleus/Scoped.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

namespace Nitrogen
{
	
	AppleEventManagerErrorsRegistrationDependency::AppleEventManagerErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterAppleEventManagerErrors();
	
	
	class AppleEventManagerErrorsRegistration
	{
		public:
			AppleEventManagerErrorsRegistration()  { RegisterAppleEventManagerErrors(); }
	};
	
	static AppleEventManagerErrorsRegistration theRegistration;
	
	
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
	
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall )
	{
		ThrowOSStatus( ::AEInstallCoercionHandler( toInstall.fromType, 
		                                           toInstall.toType, 
		                                           toInstall.handler, 
		                                           toInstall.handlerRefCon, 
		                                           toInstall.fromTypeIsDesc,
		                                           toInstall.isSysHandler ) );
		
		return Nucleus::Owned< AECoercionHandler >::Seize( toInstall );
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
	
	Nucleus::Owned< AEDesc > AECoercePtr( DescType     typeCode, 
	                                      const void*  dataPtr, 
	                                      Size         dataSize, 
	                                      DescType     toType )
	{
		// Necessary for OS 9; OS X does this automatically
		if ( toType == typeWildCard )
		{
			toType = typeCode;
		}
		
		AEDesc result;
		
		ThrowOSStatus( ::AECoercePtr( typeCode,
		                              dataPtr,
		                              dataSize,
		                              toType,
		                              &result ) );
		
		return Nucleus::Owned< AEDesc >::Seize( result );
	}
	
	Nucleus::Owned< AEDesc > AECoerceDesc( const AEDesc& desc, DescType toType )
	{
		// Necessary for OS 9; OS X does this automatically
		if ( toType == typeWildCard )
		{
			toType = desc.descriptorType;
		}
		
		AEDesc result;
		
		ThrowOSStatus( ::AECoerceDesc( &desc, toType, &result ) );
		
		return Nucleus::Owned< AEDesc >::Seize( result );
	}
	
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
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType     typeCode,
	                                       const void*  dataPtr,
	                                       Size         dataSize )
	{
		if ( dataPtr == NULL )
		{
			throw ParamErr();  // should assert?
		}
		
		AEDesc desc;
		
		ThrowOSStatus( ::AECreateDesc( typeCode, dataPtr, dataSize, &desc ) );
		
		return Nucleus::Owned< AEDesc >::Seize( desc );
	}
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType typeCode, Handle handle )
	{
		if ( handle.Get() == NULL )
		{
			throw ParamErr();  // should assert?
		}
		
		Nucleus::Scoped< HandleState > scopedHandleState( HandleState( handle ) );
		
		HLock( handle );
		
		return AECreateDesc( typeCode, *handle, GetHandleSize( handle ) );
	}
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType typeCode, Nucleus::Owned< Handle > handle )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		return AECreateDesc( typeCode, handle.Get() );
		
	#else
		
		AEDesc desc;
		
		desc.descriptorType = typeCode;
		desc.dataHandle     = handle.Release();
		
		return Nucleus::Owned< AEDesc >::Seize( desc );
		
	#endif
	}
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType                  typeCode,
	                                       Nucleus::Owned< AEDesc >  desc )
	{
		Detail::AEDescEditor( desc ).Get().descriptorType = typeCode;
		
		return desc;
	}
	
	Nucleus::Owned< AEDesc > AEDuplicateDesc( const AEDesc& desc )
	{
		AEDesc result;
		
		ThrowOSStatus( ::AEDuplicateDesc( &desc, &result ) );
		
		return Nucleus::Owned< AEDesc >::Seize( result );
	}
	
	Nucleus::Owned< AEDesc > AECreateList( bool isRecord )
	{
		return AECreateList( NULL, 0, isRecord );
	}
	
	Nucleus::Owned< AEDesc > AECreateList( const void*  factoringPtr, 
	                                       std::size_t  factoredSize, 
	                                       bool         isRecord )
	{
		AEDesc desc;
		
		ThrowOSStatus( ::AECreateList( factoringPtr, factoredSize, isRecord, &desc ) );
		
		return Nucleus::Owned< AEDesc >::Seize( desc );
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
	
	void AEPutPtr( Nucleus::Owned< AEDescList >&  list, 
	               long                           index, 
	               DescType                       type, 
	               const void*                    dataPtr, 
	               Size                           dataSize )
	{
		AEPutPtr( Detail::AEDescEditor( list ),
		          index,
		          type,
		          dataPtr,
		          dataSize );
	}
	
	void AEPutDesc( AEDescList& list, long index, const AEDesc& desc )
	{
		ThrowOSStatus( ::AEPutDesc( &list, index, &desc ) );
	}
	
	void AEPutDesc( Nucleus::Owned< AEDescList >&  list,
	                long                           index,
	                const AEDesc&                  desc )
	{
		AEPutDesc( Detail::AEDescEditor( list ), index, desc );
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
		
		result.keyword    = keyword;
		result.typeCode   = typeCode;
		result.actualSize = actualSize;
		
		return result;
	}
	
	Nucleus::Owned< AEDesc > AEGetNthDesc( const AEDesc&  listDesc, 
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
		
		return Nucleus::Owned< AEDesc >::Seize( result );
	}
	
	Nucleus::Owned< AEDesc > AEGetNthDesc( const AEDesc&  listDesc, 
	                                       long           index, 
	                                       ::AEKeyword*   keywordResult )
	{
		return AEGetNthDesc( listDesc, index, typeWildCard, keywordResult );
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
		
		result.typeCode = typeCode;
		result.dataSize = dataSize;
		
		return result;
	}
	
	void AEDeleteItem( AEDescList& list, long index )
	{
		ThrowOSStatus( ::AEDeleteItem( &list, index ) );
	}
	
	void AEDeleteItem( Nucleus::Owned< AEDescList >& list, long index )
	{
		AEDeleteItem( Detail::AEDescEditor( list ), index );
	}
	
	void AEPutKeyPtr( AERecord&    record,
	                  AEKeyword    keyword,
	                  DescType     typeCode,
	                  const void*  dataPtr,
	                  std::size_t  dataSize )
	{
		Nucleus::Scoped< ::DescType& > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEPutKeyPtr( &record,
		                              keyword,
		                              typeCode,
		                              dataPtr,
		                              dataSize ) );
	}
	
	void AEPutKeyPtr( Nucleus::Owned< AERecord >&  record,
	                  AEKeyword                    keyword,
	                  DescType                     typeCode,
	                  const void*                  dataPtr,
	                  std::size_t                  dataSize )
	{
		AEPutKeyPtr( Detail::AEDescEditor( record ),
		             keyword,
		             typeCode,
		             dataPtr,
		             dataSize );
	}
	
	void AEPutKeyDesc( AERecord&      record,
	                   AEKeyword      keyword,
	                   const AEDesc&  desc )
	{
		Nucleus::Scoped< ::DescType& > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEPutKeyDesc( &record, keyword, &desc ) );
	}
	
	void AEPutKeyDesc( AERecord&         record,
	                   const AEKeyDesc&  keyDesc )
	{
		AEPutKeyDesc( record, keyDesc.descKey, keyDesc.descContent );;
	}
	
	void AEPutKeyDesc( Nucleus::Owned< AERecord >&  record,
	                   AEKeyword                    keyword,
	                   const AEDesc&                desc )
	{
		AEPutKeyDesc( Detail::AEDescEditor( record ), keyword, desc );
	}
	
	void AEPutKeyDesc( Nucleus::Owned< AERecord >&  record,
	                   const AEKeyDesc&             keyDesc )
	{
		AEPutKeyDesc( Detail::AEDescEditor( record ), keyDesc );
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
	
	Nucleus::Owned< AEDesc > AEGetKeyDesc( const AERecord&  record,
	                                       AEKeyword        keyword,
	                                       DescType         desiredType )
	{
		AERecord fixedAERecord = Fix_AERecordDescriptorType( record );
		
		AEDesc result;
		
		ThrowOSStatus( ::AEGetKeyDesc( &fixedAERecord,
		                               keyword,
		                               desiredType,
		                               &result ) );
		
		return Nucleus::Owned< AEDesc >::Seize( result );
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
		Nucleus::Scoped< ::DescType& > scopedDescType( record.descriptorType, typeAERecord );
		
		ThrowOSStatus( ::AEDeleteKeyDesc( &record, keyword ) );
	}
	
	void AEDeleteKeyDesc( Nucleus::Owned< AERecord >&  record, 
	                      AEKeyword                    keyword )
	{
		AEDeleteKeyDesc( Detail::AEDescEditor( record ), keyword );
	}
	
	Nucleus::Owned< AppleEvent > AECreateAppleEvent( AEEventClass          eventClass,
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
		
		return Nucleus::Owned< AppleEvent >::Seize( result );
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
	
	void AEPutParamPtr( Nucleus::Owned< AppleEvent >&  appleEvent,
	                    AEKeyword                      keyword,
	                    DescType                       typeCode,
	                    const void*                    dataPtr,
	                    std::size_t                    dataSize )
	{
		AEPutParamPtr( Detail::AEDescEditor( appleEvent ),
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
	
	void AEPutParamDesc( Nucleus::Owned< AppleEvent >&  appleEvent,
	                     AEKeyword                      keyword,
	                     const AEDesc&                  desc )
	{
		AEPutParamDesc( Detail::AEDescEditor( appleEvent ), keyword, desc );
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
	
	Nucleus::Owned< AEDesc > AEGetParamDesc( const AppleEvent&  appleEvent,
	                                         AEKeyword          keyword,
	                                         DescType           desiredType )
	{
		AEDesc result;
		
		ThrowOSStatus( ::AEGetParamDesc( &appleEvent,
		                                 keyword,
		                                 desiredType,
		                                 &result ) );
		
		return Nucleus::Owned< AEDesc >::Seize( result );
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
	
	void AEDeleteParam( Nucleus::Owned< AppleEvent >&  appleEvent,
	                    AEKeyword                      keyword )
	{
		AEDeleteParam( Detail::AEDescEditor( appleEvent ), keyword );
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
	
	void AEPutAttributePtr( Nucleus::Owned< AppleEvent >&  appleEvent,
	                        AEKeyword                      keyword,
	                        DescType                       typeCode,
	                        const void*                    dataPtr,
	                        std::size_t                    dataSize )
	{
		AEPutAttributePtr( Detail::AEDescEditor( appleEvent ),
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
	
	void AEPutAttributeDesc( Nucleus::Owned< AppleEvent >&  appleEvent,
	                         AEKeyword                      keyword,
	                         const AEDesc&                  desc )
	{
		AEPutAttributeDesc( Detail::AEDescEditor( appleEvent ), keyword, desc );
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
	
	Nucleus::Owned< AEDesc > AEGetAttributeDesc( const AppleEvent&  appleEvent,
	                                             AEKeyword          keyword,
	                                             DescType           desiredType )
	{
		AEDesc result;
		
		ThrowOSStatus( ::AEGetAttributeDesc( &appleEvent,
		                                     keyword,
		                                     desiredType,
		                                     &result ) );
		
		return Nucleus::Owned< AEDesc >::Seize( result );
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
	
	void AEReplaceDescData( DescType                   typeCode,
	                        const void*                dataPtr,
	                        std::size_t                dataSize,
	                        Nucleus::Owned< AEDesc >&  result)
	{
		AEReplaceDescData( typeCode, dataPtr, dataSize, Detail::AEDescEditor( result ) );
	}
	
}

