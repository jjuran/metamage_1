// AEDataModel.h

#ifndef NITROGEN_AEDATAMODEL_H
#define NITROGEN_AEDATAMODEL_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEDATAMODEL__
#include FRAMEWORK_HEADER(AE,AEDataModel.h)
#endif
#ifndef __EVENTS__
#include FRAMEWORK_HEADER(HIToolbox,Events.h)
#endif

#if CALL_NOT_IN_CARBON
#ifndef __EPPC__
#include <EPPC.h>
#endif
#endif

#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif
#ifndef NITROGEN_REFCON_H
#include "Nitrogen/RefCon.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_PROCESSES_H
#include "Nitrogen/Processes.h"
#endif

#include <string>


#ifdef AEPutKeyPtr
#undef AEPutKeyPtr

inline OSErr AEPutKeyPtr(
	AppleEvent *  theAppleEvent,
	AEKeyword     theAEKeyword,
	DescType      typeCode,
	const void *  dataPtr,
	Size          dataSize)
{
	return AEPutParamPtr(theAppleEvent, theAEKeyword, typeCode, dataPtr, dataSize);
}

#endif

#ifdef AEPutKeyDesc
#undef AEPutKeyDesc

inline OSErr AEPutKeyDesc(
	AppleEvent *    theAppleEvent,
	AEKeyword       theAEKeyword,
	const AEDesc *  theAEDesc)
{
	return AEPutParamDesc(theAppleEvent, theAEKeyword, theAEDesc);
}

#endif

#ifdef AEGetKeyPtr
#undef AEGetKeyPtr

inline OSErr AEGetKeyPtr(
	const AppleEvent *  theAppleEvent,
	AEKeyword           theAEKeyword,
	DescType            desiredType,
	DescType *          typeCode,
	void *              dataPtr,
	Size                maximumSize,
	Size *              actualSize)
{
	return AEGetParamPtr(theAppleEvent, theAEKeyword, desiredType, typeCode, dataPtr, maximumSize, actualSize);
}

#endif

#ifdef AEGetKeyDesc
#undef AEGetKeyDesc

inline OSErr AEGetKeyDesc(
	const AppleEvent *  theAppleEvent,
	AEKeyword           theAEKeyword,
	DescType            desiredType,
	AEDesc *            result)
{
	return AEGetParamDesc(theAppleEvent, theAEKeyword, desiredType, result);
}

#endif

#ifdef AESizeOfKeyDesc
#undef AESizeOfKeyDesc

inline OSErr AESizeOfKeyDesc(
	const AppleEvent *  theAppleEvent,
	AEKeyword           theAEKeyword,
	DescType *          typeCode,
	Size *              dataSize)
{
	return AESizeOfParam(theAppleEvent, theAEKeyword, typeCode, dataSize);
}

#endif

#ifdef AEDeleteKeyDesc
#undef AEDeleteKeyDesc

inline OSErr AEDeleteKeyDesc(
	AppleEvent *  theAppleEvent,
	AEKeyword     theAEKeyword)
{
	return AEDeleteParam(theAppleEvent, theAEKeyword);
}

#endif


namespace Nitrogen
  {
	
	void RegisterAppleEventManagerErrors();
	
   template < ::DescType > struct DescType_Traits;
      /*
         The DescType_Traits are used according to these formulas:
         
         template < ::DescType type >
         typename DescType_Traits< type >::Result GetWhatever( ...parameters... )
           {
            typedef DescType_Traits< type > Traits;
            
            typename Traits::InputBuffer buffer;
            
            if ( Traits::inputHasVariableLength )
               Traits::SetInputBufferLength( buffer, GetWhateverLength( ...parameters... ) );
            
            GetWhateverData( Traits::InputBufferLength( buffer ),
                             Traits::InputBufferStart( buffer ),
                              ...parameters... );
            
            return Traits::ProcessInputBuffer( buffer );
           }

         template < ::DescType type >
         void PutWhatever( typename DescType_Traits< type >::Parameter toPut, ...parameters... )
           {
            typedef DescType_Traits< type > Traits;
            
            typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( toPut );
            
            PutWhateverData( Traits::OutputBufferLength( buffer ),
                             Traits::OutputBufferStart( buffer ),
                              ...parameters... );
            
            Traits::ReleaseOutputBuffer( buffer );
           }
      */
      
   template < class FinalType >
   struct POD_DescType_Traits
     {
      typedef FinalType Result;
      typedef const FinalType& Parameter;
      
      typedef FinalType InputBuffer;
      typedef const FinalType& OutputBuffer;

      static const bool inputHasVariableLength = false;
      static void SetInputBufferLength( InputBuffer&, std::size_t )         {}

      static void *      InputBufferStart(   InputBuffer& buffer )          { return &buffer; }
      static std::size_t InputBufferLength(  InputBuffer& buffer )          { return sizeof( buffer ); }
      static Result  ProcessInputBuffer( InputBuffer& buffer )              { return buffer; }
      
      static const void *OutputBufferStart(  OutputBuffer buffer )          { return &buffer; }
      static std::size_t OutputBufferLength( OutputBuffer buffer )          { return sizeof( buffer ); }
      static OutputBuffer PrepareOutputBuffer( Parameter output )           { return output; }
      static void ReleaseOutputBuffer( OutputBuffer )                       {}
     };

   template < class FinalType, class OriginalType >
   struct Converting_DescType_Traits
     {
      typedef FinalType Result;
      typedef const FinalType& Parameter;
      
      typedef OriginalType       InputBuffer;
      typedef const OriginalType OutputBuffer;

      static const bool inputHasVariableLength = false;
      static void SetInputBufferLength( InputBuffer&, std::size_t )         {}

      static void *      InputBufferStart(  InputBuffer& buffer )           { return &buffer; }
      static std::size_t InputBufferLength( InputBuffer& buffer )           { return sizeof( buffer ); }
      static Result  ProcessInputBuffer( InputBuffer& buffer )              { return buffer; }

      static const void *OutputBufferStart(  const OutputBuffer& buffer )   { return &buffer; }
      static std::size_t OutputBufferLength( const OutputBuffer& buffer )   { return sizeof( buffer ); }
      static OutputBuffer PrepareOutputBuffer( Parameter output )           { return output; }
      static void ReleaseOutputBuffer( OutputBuffer& )                      {}
     };

   template< class Ownable >
   struct Seizing_DescType_Traits
     {
      typedef Owned< Ownable > Result;
      typedef Ownable Parameter;
      
      typedef Ownable        InputBuffer;
      typedef const Ownable& OutputBuffer;

      static const bool inputHasVariableLength = false;
      static void SetInputBufferLength( InputBuffer&, std::size_t )          {}

      static void *      InputBufferStart(  InputBuffer& buffer )            { return &buffer; }
      static std::size_t InputBufferLength( InputBuffer& buffer )            { return sizeof( buffer ); }
      static Result   ProcessInputBuffer( InputBuffer& buffer )              { return Owned< Ownable >::Seize( buffer ); }

      static const void *OutputBufferStart(  OutputBuffer buffer )           { return &buffer; }
      static std::size_t OutputBufferLength( OutputBuffer buffer )           { return sizeof( buffer ); }
      static OutputBuffer PrepareOutputBuffer( const Parameter& output )     { return output; }
      static void ReleaseOutputBuffer( OutputBuffer )                        {}
     };

   template< class POD, std::size_t (*SizeOf)( const POD& ) >
   struct VariableLengthPOD_DescType_Traits
     {
      typedef std::auto_ptr< POD > Result;
      typedef const POD& Parameter;
      
      struct InputBuffer
        {
         std::size_t length;
         std::auto_ptr< POD > result;
        };
      
      typedef const POD& OutputBuffer;

      static const bool inputHasVariableLength = true;
      
      static void SetInputBufferLength( InputBuffer& b, std::size_t s )    
        {
         b.length = s;
         b.result = std::auto_ptr< POD >( static_cast<POD*>( ::operator new( s ) ) );
        }

      static void *      InputBufferStart(  InputBuffer& buffer )           { return buffer.result.get(); }
      static std::size_t InputBufferLength( InputBuffer& buffer )           { return buffer.length; }
      static Result  ProcessInputBuffer( InputBuffer& buffer )              { return buffer.result; }

      static const void *OutputBufferStart(  OutputBuffer buffer )          { return &buffer; }
      static std::size_t OutputBufferLength( OutputBuffer buffer )          { return SizeOf( buffer ); }
      static OutputBuffer PrepareOutputBuffer( Parameter output )           { return output; }
      static void ReleaseOutputBuffer( OutputBuffer )                       {}
     };

   template<> struct DescType_Traits< typeChar >
     {
      typedef std::string Result;
      typedef const std::string& Parameter;
      
      typedef std::string        InputBuffer;
      typedef const std::string& OutputBuffer;

      static const bool inputHasVariableLength = true;
      static void SetInputBufferLength( InputBuffer& b, std::size_t s )     { b.resize( s ); }

      static void *      InputBufferStart(  InputBuffer& buffer )           { return buffer.empty() ? 0 : &buffer[0]; }
      static std::size_t InputBufferLength( InputBuffer& buffer )           { return buffer.size(); }
      static Result  ProcessInputBuffer( InputBuffer& buffer )              { return buffer; }

      static const void *OutputBufferStart(  OutputBuffer buffer )          { return buffer.data(); }
      static std::size_t OutputBufferLength( OutputBuffer buffer )          { return buffer.size(); }
      static OutputBuffer PrepareOutputBuffer( Parameter output )           { return output; }
      static void ReleaseOutputBuffer( OutputBuffer )                       {}
     };

   template<> struct DescType_Traits< typeFixed >
     {
      typedef double Result;
      typedef double Parameter;
      
      typedef ::Fixed InputBuffer;
      typedef ::Fixed OutputBuffer;

      static const bool inputHasVariableLength = false;
      static void SetInputBufferLength( InputBuffer&, std::size_t )         {}

      static void *      InputBufferStart(  InputBuffer& buffer )           { return &buffer; }
      static std::size_t InputBufferLength( InputBuffer& buffer )           { return sizeof( buffer ); }
      static Result ProcessInputBuffer( InputBuffer& buffer )               { return FixedToDouble( buffer ); }

      static const void *OutputBufferStart(  OutputBuffer buffer )          { return &buffer; }
      static std::size_t OutputBufferLength( OutputBuffer buffer )          { return sizeof( buffer ); }
      static OutputBuffer PrepareOutputBuffer( Parameter output )           { return DoubleToFixed( output ); }
      static void ReleaseOutputBuffer( OutputBuffer )                       {}
     };

   class DescTypeTag {};
   typedef SelectorType< DescTypeTag, ::DescType > DescType;
   
   class AEKeywordTag {};
   typedef SelectorType< AEKeywordTag, ::AEKeyword > AEKeyword;

   class AEEnumeratedTag {};
   typedef SelectorType< AEEnumeratedTag, UInt32 > AEEnumerated;
   typedef AEEnumerated AEEnumeration;
   
   class AEEventClassTag {};
   typedef SelectorType< AEEventClassTag, ::AEEventClass > AEEventClass;

   class AEEventIDTag {};
   typedef SelectorType< AEEventIDTag, ::AEEventID > AEEventID;
   
   template<> struct DescType_Traits< typeNull >                   { typedef void Result; };
   
   template<> struct DescType_Traits< typeBoolean >                : Converting_DescType_Traits< bool, ::Boolean >        {};
   template<> struct DescType_Traits< typeSInt16 >                 : POD_DescType_Traits< SInt16 >                        {};
   template<> struct DescType_Traits< typeSInt32 >                 : POD_DescType_Traits< SInt32 >                        {};
   template<> struct DescType_Traits< typeUInt32 >                 : POD_DescType_Traits< UInt32 >                        {};
   template<> struct DescType_Traits< typeSInt64 >                 : POD_DescType_Traits< SInt64 >                        {};
   template<> struct DescType_Traits< typeIEEE32BitFloatingPoint > : POD_DescType_Traits< float >                         {};
   template<> struct DescType_Traits< typeIEEE64BitFloatingPoint > : POD_DescType_Traits< double >                        {};
   template<> struct DescType_Traits< type128BitFloatingPoint >    : POD_DescType_Traits< long double >                   {};

   template<> struct DescType_Traits< typeEventRecord >            : POD_DescType_Traits< EventRecord >                   {};
   template<> struct DescType_Traits< typeEnumerated >             : Converting_DescType_Traits< AEEnumerated, UInt32 >   {};
   template<> struct DescType_Traits< typeType >                   : Converting_DescType_Traits< DescType, ::DescType >   {};
   template<> struct DescType_Traits< typeAppParameters >          : VariableLengthPOD_DescType_Traits< AppParameters, SizeOf_AppParameters > {};
   template<> struct DescType_Traits< typeFSS >                    : POD_DescType_Traits< FSSpec >                        {};
   template<> struct DescType_Traits< typeFSRef >                  : POD_DescType_Traits< FSRef >                         {};
   template<> struct DescType_Traits< typeKeyword >                : Converting_DescType_Traits< AEKeyword, ::AEKeyword > {};
   template<> struct DescType_Traits< typeApplSignature >          : Converting_DescType_Traits< OSType, ::OSType >       {};
   template<> struct DescType_Traits< typeQDRectangle >            : POD_DescType_Traits< Rect >                          {};
   template<> struct DescType_Traits< typeProcessSerialNumber >    : POD_DescType_Traits< ProcessSerialNumber >           {};
   // TargetID is defined for Carbon, but typeTargetID is not.
#if CALL_NOT_IN_CARBON
   template<> struct DescType_Traits< typeTargetID >               : POD_DescType_Traits< TargetID >                      {};
#endif

	using ::AEDesc;
	using ::AEDescList;
	using ::AEAddressDesc;
	using ::AERecord;
	using ::AppleEvent;
	
	template <>
	struct Disposer< AEDesc > : public std::unary_function< AEDesc, void >, 
	                            private DefaultDestructionOSStatusPolicy
	{
		// parameter can't be const
		void operator()( AEDesc desc ) const
		{
			// AEDisposeDesc() is documented as only ever returning noErr, 
			// but we check anyway to be future-proof.
			
			OnlyOnce< RegisterAppleEventManagerErrors >();
			DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::AEDisposeDesc( &desc ) );
		}
	};
	
	template <>
	struct LivelinessTraits< AEDesc, Disposer< AEDesc > >   { typedef SeizedValuesAreLive LivelinessTest; };
	
	template <>
	struct Maker< AEDesc >
	{
		AEDesc operator()() const
		{
			AEDesc result = { typeNull, NULL };
			return result;
		}
	};
  	
	struct GetNthPtr_Result {
		::AEKeyword keyword;
		::DescType typeCode;
		::Size actualSize;
		
		GetNthPtr_Result() : keyword(), typeCode(), actualSize() {}
		GetNthPtr_Result(AEKeyword keyword, DescType typeCode, ::Size actualSize) 
		  : keyword(keyword), typeCode(typeCode), actualSize(actualSize)  {}
	};
	
	struct AEDescInfo {
		::DescType typeCode;
		::Size dataSize;
		
		AEDescInfo() : typeCode(), dataSize() {}
		AEDescInfo(DescType typeCode, ::Size dataSize) : typeCode(typeCode), dataSize(dataSize)  {}
		
		operator ::Size() const  { return dataSize; }
	};
	typedef AEDescInfo 
		AESizeOfNthItem_Result, 
		AESizeOfKeyDesc_Result, 
		AEGetKeyPtr_Result, 
		AESizeOfParam_Result, 
		AESizeOfAttribute_Result, 
		AEGetParamPtr_Result, 
		AEGetAttributePtr_Result;

	
	void RegisterAppleEventManagerErrors();
	
	Owned< AEDesc > AECoercePtr(DescType typeCode, const void* dataPtr, Size dataSize, DescType toType);
	
	Owned< AEDesc > AECoerceDesc(const AEDesc& desc, DescType toType);
	
	inline Owned< AEDesc > AEInitializeDesc()
	{
		return Owned< AEDesc >::Seize( Make< AEDesc >() );
	}
	
	Owned< AEDesc > AECreateDesc(DescType typeCode, const void* dataPtr, Size dataSize);
	Owned< AEDesc > AECreateDesc(DescType typeCode, ::Handle handle);
	Owned< AEDesc > AECreateDesc(DescType typeCode, Owned< Handle > handle);
	
	Owned< AEDesc > AECreateDesc(DescType typeCode, Owned< AEDesc > desc);
	
	template < class Data >
	Owned< AEDesc > AECreateDesc(DescType typeCode, Data** handle)
	{
		return AECreateDesc( typeCode, reinterpret_cast< ::Handle >( handle ) );
	}
	
	
	inline void AEDisposeDesc( Owned< AEDesc > )  {}
	
	Owned< AEDesc > AEDuplicateDesc( const AEDesc& desc );
	
	Owned< AEDesc > AECreateList(bool isRecord);
	Owned< AEDesc > AECreateList(const void* factoringPtr, std::size_t factoredSize, bool isRecord);
	
	template < bool isRecord >
	Owned< AEDesc > AECreateList()  { return AECreateList( isRecord ); }
	
	template < bool isRecord >
	Owned< AEDesc > AECreateList(DescType typeCode)
	{
		return AECreateDesc( typeCode, AECreateList< isRecord >() );
	}
	
	long AECountItems(const AEDesc& desc);
	
	void AEPutPtr(        AEDescList  & list, long index, DescType type, const void* dataPtr, Size dataSize);
	void AEPutPtr( Owned< AEDescList >& list, long index, DescType type, const void* dataPtr, Size dataSize);
	
	void AEPutDesc(        AEDescList  & list, long index, const AEDesc& desc );
	void AEPutDesc( Owned< AEDescList >& list, long index, const AEDesc& desc );
	
	GetNthPtr_Result AEGetNthPtr(
		const AEDesc& listDesc, 
		long index, 
		DescType desiredType, 
		void* dataPtr, 
		Size maximumSize
	);
	
	Owned< AEDesc > AEGetNthDesc(
		const AEDesc& listDesc, 
		long index, 
		DescType desiredType = typeWildCard, 
		::AEKeyword* keyword = NULL
	);
	
	Owned< AEDesc > AEGetNthDesc(
		const AEDesc& listDesc, 
		long index, 
		::AEKeyword* keyword
	);
	
	Size AESizeOfNthItem(const AEDescList& list, long index);
	
	void AEDeleteItem(        AEDescList  & listDesc, long index );
	void AEDeleteItem( Owned< AEDescList >& listDesc, long index );
	
	inline bool AECheckIsRecord( const AEDesc& theDesc )
	{
		return ::AECheckIsRecord( &theDesc );
	}
	
	void AEPutKeyPtr(
		AERecord& record, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize
	);
	
	void AEPutKeyPtr(
		Owned< AERecord >& record, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize
	);
	
	void AEPutKeyDesc(
		AERecord& record, 
		AEKeyword keyword, 
		const AEDesc& desc
	);
	
	void AEPutKeyDesc(
		Owned< AERecord >& record, 
		AEKeyword keyword, 
		const AEDesc& desc
	);
	
	AEGetKeyPtr_Result AEGetKeyPtr(
		const AERecord& record, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize
	);
	
	Owned< AEDesc > AEGetKeyDesc(
		const AERecord& record, 
		AEKeyword keyword, 
		DescType desiredType
	);
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc(
		const AERecord& record, 
		AEKeyword keyword
	);
	
	void AEDeleteKeyDesc(        AERecord  & record, AEKeyword keyword );
	void AEDeleteKeyDesc( Owned< AERecord >& record, AEKeyword keyword );
	
	Owned< AppleEvent > AECreateAppleEvent(
		AEEventClass eventClass, 
		AEEventID eventID, 
		const AEAddressDesc& target, 
		AEReturnID returnID = AEReturnID(), 
		AETransactionID transactionID = AETransactionID()
	);
	
	void AEPutParamPtr(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize
	);
	
	void AEPutParamPtr(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize
	);
	
	void AEPutParamDesc(        AppleEvent  & appleEvent, AEKeyword keyword, const AEDesc& desc );
	void AEPutParamDesc( Owned< AppleEvent >& appleEvent, AEKeyword keyword, const AEDesc& desc );
	
	AEGetParamPtr_Result AEGetParamPtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize
	);
	
	Owned< AEDesc > AEGetParamDesc(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType
	);
	
	AESizeOfParam_Result AESizeOfParam(
		const AppleEvent& appleEvent, 
		AEKeyword keyword
	);
	
	void AEDeleteParam(        AppleEvent  & appleEvent, AEKeyword keyword );
	void AEDeleteParam( Owned< AppleEvent >& appleEvent, AEKeyword keyword );
	
	void AEPutAttributePtr(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize
	);
	
	void AEPutAttributePtr(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize
	);
	
	void AEPutAttributeDesc(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc
	);
	
	void AEPutAttributeDesc(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		const AEDesc& desc
	);
	
	AEGetAttributePtr_Result AEGetAttributePtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType, 
		void* dataPtr, 
		std::size_t maximumSize
	);
	
	Owned< AEDesc > AEGetAttributeDesc(
		const AppleEvent& appleEvent, 
		AEKeyword keyword, 
		DescType desiredType
	);
	
	AESizeOfAttribute_Result AESizeOfAttribute(
		const AppleEvent& appleEvent, 
		AEKeyword keyword
	);
	
	void AEGetDescData( const AEDesc& desc, void* dataPtr, std::size_t maximumSize );
	
	inline std::size_t AEGetDescDataSize( const AEDesc& desc )
	{
		return ::AEGetDescDataSize( &desc );
	}
	
	void AEReplaceDescData(
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize, 
		AEDesc& result
	);
	
	void AEReplaceDescData(
		DescType typeCode, 
		const void* dataPtr, 
		std::size_t dataSize, 
		Owned< AEDesc >& result
	);
	
	
	template < ::DescType type >
	Owned< AEDesc > AECoercePtr(typename DescType_Traits< type >::Parameter data, DescType toType)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		Owned< AEDesc > result = AECoercePtr(
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer ), 
			toType
		);
		
		Traits::ReleaseOutputBuffer( buffer );
		
		return result;
	}
	
	template < ::DescType type >
	Owned< AEDesc > AECreateDesc(typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		Owned< AEDesc > desc = AECreateDesc(
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
		
		return desc;
	}
	
	template < ::DescType type >
	void AEPutPtr(
		AEDescList& list, 
		long index, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutPtr(
			list, 
			index, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutPtr(
		Owned< AEDescList >& list, 
		long index, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutPtr(
			list, 
			index, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AEGetNthPtr(
		const AEDescList& listDesc, 
		long index
	)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
			Traits::SetInputBufferLength( buffer, AESizeOfNthItem( listDesc, index ) );
		
		AEGetNthPtr(
			listDesc, 
			index, 
			type, 
			Traits::InputBufferStart( buffer ),
			Traits::InputBufferLength( buffer )
		);
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutKeyPtr(
		AERecord& record, 
		AEKeyword keyword, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutKeyPtr(
			record, 
			keyword, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutKeyPtr(
		Owned< AERecord >& record, 
		AEKeyword keyword, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutKeyPtr(
			record, 
			keyword, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AEGetKeyPtr(
		const AERecord& record, 
		AEKeyword keyword
	)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
			Traits::SetInputBufferLength( buffer, AESizeOfKeyDesc( record, keyword ) );
		
		AEGetKeyPtr(
			record, 
			keyword, 
			type, 
			Traits::InputBufferStart( buffer ),
			Traits::InputBufferLength( buffer )
		);
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutParamPtr(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutParamPtr(
			appleEvent, 
			keyword, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutParamPtr(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutParamPtr(
			appleEvent, 
			keyword, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AEGetParamPtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword
	)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
			Traits::SetInputBufferLength( buffer, AESizeOfParam( appleEvent, keyword ) );
		
		AEGetParamPtr(
			appleEvent, 
			keyword, 
			type, 
			Traits::InputBufferStart( buffer ),
			Traits::InputBufferLength( buffer )
		);
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutAttributePtr(
		AppleEvent& appleEvent, 
		AEKeyword keyword, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutAttributePtr(
			appleEvent, 
			keyword, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutAttributePtr(
		Owned< AppleEvent >& appleEvent, 
		AEKeyword keyword, 
		typename DescType_Traits< type >::Parameter data)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutAttributePtr(
			appleEvent, 
			keyword, 
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer )
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AEGetAttributePtr(
		const AppleEvent& appleEvent, 
		AEKeyword keyword
	)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
			Traits::SetInputBufferLength( buffer, AESizeOfAttribute( appleEvent, keyword ) );
		
		AEGetAttributePtr(
			appleEvent, 
			keyword, 
			type, 
			Traits::InputBufferStart( buffer ),
			Traits::InputBufferLength( buffer )
		);
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AEGetDescData( const AEDesc& desc )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
			Traits::SetInputBufferLength( buffer, AEGetDescDataSize( desc ) );
		
		AEGetDescData(
			desc, 
			Traits::InputBufferStart( buffer ),
			Traits::InputBufferLength( buffer )
		);
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEReplaceDescData( typename DescType_Traits< type >::Parameter data, AEDesc& result )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEReplaceDescData(
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer ), 
			result
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}

	template < ::DescType type >
	void AEReplaceDescData(typename DescType_Traits< type >::Parameter data, Owned< AEDesc >& result)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEReplaceDescData(
			type, 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer ), 
			result
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}

 
   struct AEEventHandlerUPP_Details: Basic_UPP_Details< ::AEEventHandlerUPP,
                                                        ::AEEventHandlerProcPtr,
                                                        ::NewAEEventHandlerUPP,
                                                        ::DisposeAEEventHandlerUPP,
                                                        ::InvokeAEEventHandlerUPP >
     {};
   
   typedef UPP< AEEventHandlerUPP_Details > AEEventHandlerUPP;
   
   using ::AEEventHandlerProcPtr;
   
   inline Owned< AEEventHandlerUPP > NewAEEventHandlerUPP( AEEventHandlerProcPtr p )
     {
      return NewUPP< AEEventHandlerUPP >( p );
     }

   inline void DisposeAEEventHandlerUPP( Owned< AEEventHandlerUPP > )
     {
     }
   
   inline void InvokeAEEventHandlerUPP( const AppleEvent& theAppleEvent,
                                        AppleEvent& reply,
                                        RefCon handlerRefCon,
                                        AEEventHandlerUPP userUPP )
     {
      ThrowOSStatus( userUPP( &theAppleEvent, &reply, handlerRefCon ) );
     }
  }

#endif
