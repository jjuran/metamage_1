// AEDataModel.h

#ifndef NITROGEN_AEDATAMODEL_H
#define NITROGEN_AEDATAMODEL_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEDATAMODEL__
#include FRAMEWORK_HEADER(AE,AEDataModel.h)
#endif
#ifndef __AEREGISTRY__
#include FRAMEWORK_HEADER(Carbon,AERegistry.h)
#endif
#ifndef __EVENTS__
#include FRAMEWORK_HEADER(HIToolbox,Events.h)
#endif
#ifndef __PROCESSES__
#include FRAMEWORK_HEADER(HIServices,Processes.h)
#endif

#if !TARGET_API_MAC_CARBON
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

#include <string>


#undef AEPutKeyPtr
#undef AEPutKeyDesc
#undef AEGetKeyPtr
#undef AEGetKeyDesc
#undef AESizeOfKeyDesc
#undef AEDeleteKeyDesc

inline OSErr AEPutKeyPtr(
	AppleEvent *  theAppleEvent,
	AEKeyword     theAEKeyword,
	DescType      typeCode,
	const void *  dataPtr,
	Size          dataSize)
{
	return AEPutParamPtr(theAppleEvent, theAEKeyword, typeCode, dataPtr, dataSize);
}

inline OSErr AEPutKeyDesc(
	AppleEvent *    theAppleEvent,
	AEKeyword       theAEKeyword,
	const AEDesc *  theAEDesc)
{
	return AEPutParamDesc(theAppleEvent, theAEKeyword, theAEDesc);
}

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

inline OSErr AEGetKeyDesc(
	const AppleEvent *  theAppleEvent,
	AEKeyword           theAEKeyword,
	DescType            desiredType,
	AEDesc *            result)
{
	return AEGetParamDesc(theAppleEvent, theAEKeyword, desiredType, result);
}

inline OSErr AESizeOfKeyDesc(
	const AppleEvent *  theAppleEvent,
	AEKeyword           theAEKeyword,
	DescType *          typeCode,
	Size *              dataSize)
{
	return AESizeOfParam(theAppleEvent, theAEKeyword, typeCode, dataSize);
}

inline OSErr AEDeleteKeyDesc(
	AppleEvent *  theAppleEvent,
	AEKeyword     theAEKeyword)
{
	return AEDeleteParam(theAppleEvent, theAEKeyword);
}


namespace Nitrogen
  {
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
   template<> struct DescType_Traits< typeFSS >                    : POD_DescType_Traits< FSSpec >                        {};
   template<> struct DescType_Traits< typeFSRef >                  : POD_DescType_Traits< FSRef >                         {};
   template<> struct DescType_Traits< typeKeyword >                : Converting_DescType_Traits< AEKeyword, ::AEKeyword > {};
   template<> struct DescType_Traits< typeApplSignature >          : Converting_DescType_Traits< OSType, ::OSType >       {};
   template<> struct DescType_Traits< typeQDRectangle >            : POD_DescType_Traits< Rect >                          {};
   template<> struct DescType_Traits< typeProcessSerialNumber >    : POD_DescType_Traits< ProcessSerialNumber >           {};
   template<> struct DescType_Traits< typeRGBColor >               : POD_DescType_Traits< RGBColor >                      {};
#if !TARGET_API_MAC_CARBON
   template<> struct DescType_Traits< typeTargetID >               : POD_DescType_Traits< TargetID >                      {};
#endif

	using ::AEDesc;
	using ::AEDescList;
	using ::AERecord;
	using ::AppleEvent;
	
	template <>
	struct Disposer< AEDesc > : public std::unary_function< AEDesc, void > {
		// parameter can't be const
		void operator()( AEDesc desc ) const
		{
			::AEDisposeDesc( &desc );
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
		Size actualSize;
		
		GetNthPtr_Result() : keyword(), typeCode(), actualSize() {}
		GetNthPtr_Result(AEKeyword keyword, DescType typeCode, Size actualSize) 
		  : keyword(keyword), typeCode(typeCode), actualSize(actualSize)  {}
	};
	
	struct AEDescInfo {
		::DescType typeCode;
		Size dataSize;
		
		AEDescInfo() : typeCode(), dataSize() {}
		AEDescInfo(DescType typeCode, Size dataSize) : typeCode(typeCode), dataSize(dataSize)  {}
		
		operator Size() const  { return dataSize; }
	};
	typedef AEDescInfo 
		AESizeOfNthItem_Result, 
		AESizeOfKeyDesc_Result, 
		AEGetKeyPtr_Result, 
		AESizeOfParam_Result, 
		AESizeOfAttribute_Result, 
		AEGetParamPtr_Result, 
		AEGetAttributePtr_Result;
 
   struct AEEventHandlerUPP_Details: Basic_UPP_Details< ::AEEventHandlerUPP,
                                                        ::AEEventHandlerProcPtr,
                                                        ::NewAEEventHandlerUPP,
                                                        ::DisposeAEEventHandlerUPP,
                                                        ::InvokeAEEventHandlerUPP >
     {};
   
   typedef UPP< AEEventHandlerUPP_Details > AEEventHandlerUPP;
   
   using ::AEEventHandlerProcPtr;
   
   inline Owned<AEEventHandlerUPP> NewAEEventHandlerUPP( AEEventHandlerProcPtr p )
     {
      return NewUPP<AEEventHandlerUPP>( p );
     }

   inline void DisposeAEEventHandlerUPP( Owned<AEEventHandlerUPP> )
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
