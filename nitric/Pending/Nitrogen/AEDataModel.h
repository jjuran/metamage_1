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
#ifndef NITROGEN_ALIASES_H
#include "Nitrogen/Aliases.h"
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
	
	#pragma mark -
	#pragma mark ¥ DescType ¥
	
	class DescType_Tag {};
	typedef SelectorType< DescType_Tag, ::DescType > DescType;
	
	// Apple event descriptor types
	inline DescType TypeBoolean               ()  { return DescType::Make( typeBoolean                ); }
	inline DescType TypeChar                  ()  { return DescType::Make( typeChar                   ); }
	
	// Preferred numeric Apple event descriptor types
	inline DescType TypeSInt16                ()  { return DescType::Make( typeSInt16                 ); }
	inline DescType TypeSInt32                ()  { return DescType::Make( typeSInt32                 ); }
	inline DescType TypeUInt32                ()  { return DescType::Make( typeUInt32                 ); }
	inline DescType TypeSInt64                ()  { return DescType::Make( typeSInt64                 ); }
	inline DescType TypeIEEE32BitFloatingPoint()  { return DescType::Make( typeIEEE32BitFloatingPoint ); }
	inline DescType TypeIEEE64BitFloatingPoint()  { return DescType::Make( typeIEEE64BitFloatingPoint ); }
	inline DescType Type128BitFloatingPoint   ()  { return DescType::Make( type128BitFloatingPoint    ); }
	inline DescType TypeDecimalStruct         ()  { return DescType::Make( typeDecimalStruct          ); }
	
	// Non-preferred Apple event descriptor types
	inline DescType TypeExtended              ()  { return DescType::Make( typeExtended               ); }
	
	// More Apple event descriptor types
	inline DescType TypeAEList                ()  { return DescType::Make( typeAEList                 ); }
	inline DescType TypeAERecord              ()  { return DescType::Make( typeAERecord               ); }
	inline DescType TypeAppleEvent            ()  { return DescType::Make( typeAppleEvent             ); }
	inline DescType TypeEventRecord           ()  { return DescType::Make( typeEventRecord            ); }
	inline DescType TypeTrue                  ()  { return DescType::Make( typeTrue                   ); }
	inline DescType TypeFalse                 ()  { return DescType::Make( typeFalse                  ); }
	inline DescType TypeAlias                 ()  { return DescType::Make( typeAlias                  ); }
	inline DescType TypeEnumerated            ()  { return DescType::Make( typeEnumerated             ); }
	inline DescType TypeType                  ()  { return DescType::Make( typeType                   ); }
	inline DescType TypeAppParameters         ()  { return DescType::Make( typeAppParameters          ); }
	inline DescType TypeProperty              ()  { return DescType::Make( typeProperty               ); }
	inline DescType TypeFSS                   ()  { return DescType::Make( typeFSS                    ); }
	inline DescType TypeFSRef                 ()  { return DescType::Make( typeFSRef                  ); }
	inline DescType TypeFileURL               ()  { return DescType::Make( typeFileURL                ); }
	inline DescType TypeKeyword               ()  { return DescType::Make( typeKeyword                ); }
	inline DescType TypeSectionH              ()  { return DescType::Make( typeSectionH               ); }
	inline DescType TypeWildCard              ()  { return DescType::Make( typeWildCard               ); }
	inline DescType TypeFixed                 ()  { return DescType::Make( typeFixed                  ); }
	inline DescType TypeProcessSerialNumber   ()  { return DescType::Make( typeProcessSerialNumber    ); }
	inline DescType TypeApplicationURL        ()  { return DescType::Make( typeApplicationURL         ); }
	inline DescType TypeNull                  ()  { return DescType::Make( typeNull                   ); }
	
#if CALL_NOT_IN_CARBON
	
	// Deprecated addressing modes under Carbon
	inline DescType TypeSessionID             ()  { return DescType::Make( typeSessionID              ); }
	inline DescType TypeTargetID              ()  { return DescType::Make( typeTargetID               ); }
	inline DescType TypeDispatcherID          ()  { return DescType::Make( typeDispatcherID           ); }
	
#endif
	
	class AEKeyword_Tag {};
	typedef SelectorType< AEKeyword_Tag, ::AEKeyword > AEKeyword;
	
	// Keywords for Apple event attributes
	inline AEKeyword KeyTransactionIDAttr  ()  { return AEKeyword::Make( keyTransactionIDAttr   ); }
	inline AEKeyword KeyReturnIDAttr       ()  { return AEKeyword::Make( keyReturnIDAttr        ); }
	inline AEKeyword KeyEventClassAttr     ()  { return AEKeyword::Make( keyEventClassAttr      ); }
	inline AEKeyword KeyEventIDAttr        ()  { return AEKeyword::Make( keyEventIDAttr         ); }
	inline AEKeyword KeyAddressAttr        ()  { return AEKeyword::Make( keyAddressAttr         ); }
	inline AEKeyword KeyOptionalKeywordAttr()  { return AEKeyword::Make( keyOptionalKeywordAttr ); }
	inline AEKeyword KeyTimeoutAttr        ()  { return AEKeyword::Make( keyTimeoutAttr         ); }
	inline AEKeyword KeyInteractLevelAttr  ()  { return AEKeyword::Make( keyInteractLevelAttr   ); }
	inline AEKeyword KeyEventSourceAttr    ()  { return AEKeyword::Make( keyEventSourceAttr     ); }
	inline AEKeyword KeyMissedKeywordAttr  ()  { return AEKeyword::Make( keyMissedKeywordAttr   ); }
	inline AEKeyword KeyOriginalAddressAttr()  { return AEKeyword::Make( keyOriginalAddressAttr ); }
	inline AEKeyword KeyAcceptTimeoutAttr  ()  { return AEKeyword::Make( keyAcceptTimeoutAttr   ); }
	
	// Constants used creating an AppleEvent
	
	class AEReturnID_Tag {};
	typedef IDType< AEReturnID_Tag, ::AEReturnID > AEReturnID;
	
	inline AEReturnID AutoGenerateReturnID()  { return AEReturnID::Make( kAutoGenerateReturnID ); }
	
	class AETransactionID_Tag {};
	typedef IDType< AETransactionID_Tag, ::AETransactionID > AETransactionID;
	
	inline AETransactionID AnyTransactionID()  { return AETransactionID::Make( kAnyTransactionID ); }
	
	struct AESendPriority_Tag  {};
	typedef FlagType< AESendPriority_Tag, ::AESendPriority, kAENormalPriority > AESendPriority;
	
	inline AESendPriority AENormalPriority()  { return AESendPriority::Make( kAENormalPriority ); }
	inline AESendPriority AEHighPriority  ()  { return AESendPriority::Make( kAEHighPriority   ); }
	
	struct AESendMode_Tag  {};
	typedef FlagType< AESendMode_Tag, ::AESendMode > AESendMode;
	
	inline AESendMode AENoReply              ()  { return AESendMode::Make( kAENoReply               ); }
	inline AESendMode AEQueueReply           ()  { return AESendMode::Make( kAEQueueReply            ); }
	inline AESendMode AEWaitReply            ()  { return AESendMode::Make( kAEWaitReply             ); }
	inline AESendMode AEDontReconnect        ()  { return AESendMode::Make( kAEDontReconnect         ); }
	inline AESendMode AEWantReceipt          ()  { return AESendMode::Make( kAEWantReceipt           ); }
	inline AESendMode AENeverInteract        ()  { return AESendMode::Make( kAENeverInteract         ); }
	inline AESendMode AECanInteract          ()  { return AESendMode::Make( kAECanInteract           ); }
	inline AESendMode AEAlwaysInteract       ()  { return AESendMode::Make( kAEAlwaysInteract        ); }
	inline AESendMode AECanSwitchLayer       ()  { return AESendMode::Make( kAECanSwitchLayer        ); }
	inline AESendMode AEDontRecord           ()  { return AESendMode::Make( kAEDontRecord            ); }
	inline AESendMode AEDontExecute          ()  { return AESendMode::Make( kAEDontExecute           ); }
	inline AESendMode AEProcessNonReplyEvents()  { return AESendMode::Make( kAEProcessNonReplyEvents ); }
	
	#pragma mark -
	#pragma mark ¥ DescType_Traits ¥
	
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
      static Result  ProcessInputBuffer( InputBuffer& buffer )              { return Result( buffer ); }

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

	template< class T >
	struct Handle_DescType_Traits
	{
		typedef T   DataType;
		typedef T** HandleType;
		
		typedef Owned< HandleType > Result;
		typedef HandleType Parameter;
		
		typedef Owned< Handle > InputBuffer;
		
		typedef HandleType OutputBuffer;

		static const bool inputHasVariableLength = true;
		
		static void SetInputBufferLength( InputBuffer& b, std::size_t s )    
		{
			b = NewHandle( s );
			HLock( b );
		}

		static void *      InputBufferStart(  InputBuffer& buffer )           { return *buffer.Get(); }
		static std::size_t InputBufferLength( InputBuffer& buffer )           { return GetHandleSize( buffer ); }
		static Result  ProcessInputBuffer( InputBuffer& buffer )
		{
			::HUnlock( buffer.Get() );
			return Handle_Cast< DataType >( buffer );
		}
		
		// Yes, this loses the previous locked state, which could be bad.
		// When DescType_Traits are made exception-safe, then I'll deal with it.
		
		static const void *OutputBufferStart(  OutputBuffer buffer )          { return *buffer; }
		static std::size_t OutputBufferLength( OutputBuffer buffer )          { return GetHandleSize( buffer ); }
		static OutputBuffer PrepareOutputBuffer( Parameter output )           { HLock( output );  return output; }
		static void ReleaseOutputBuffer( OutputBuffer buffer )                { ::HUnlock( (Handle)buffer ); }
	};
	
   template<> struct DescType_Traits< ::typeChar >
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

   template<> struct DescType_Traits< ::typeFixed >
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
   template<> struct DescType_Traits< typeAlias >                  : Handle_DescType_Traits< AliasRecord >                {};
   template<> struct DescType_Traits< typeEnumerated >             : Converting_DescType_Traits< AEEnumerated, UInt32 >   {};
   template<> struct DescType_Traits< typeType >                   : Converting_DescType_Traits< DescType, ::DescType >   {};
   template<> struct DescType_Traits< typeAppParameters >          : VariableLengthPOD_DescType_Traits< AppParameters, SizeOf_AppParameters > {};
   template<> struct DescType_Traits< typeFSS >                    : POD_DescType_Traits< FSSpec >                        {};
   template<> struct DescType_Traits< typeFSRef >                  : POD_DescType_Traits< FSRef >                         {};
   template<> struct DescType_Traits< typeKeyword >                : Converting_DescType_Traits< AEKeyword, ::AEKeyword > {};
   template<> struct DescType_Traits< typeApplSignature >          : Converting_DescType_Traits< OSType, ::OSType >       {};
   template<> struct DescType_Traits< typeQDRectangle >            : POD_DescType_Traits< Rect >                          {};
   template<> struct DescType_Traits< typeProcessSerialNumber >    : POD_DescType_Traits< ProcessSerialNumber >           {};
   template<> struct DescType_Traits< typeApplicationURL >         : DescType_Traits< typeChar >                          {};
   
	

   // TargetID is defined for Carbon, but typeTargetID is not.
#if CALL_NOT_IN_CARBON
   template<> struct DescType_Traits< typeTargetID >               : POD_DescType_Traits< TargetID >                      {};
#endif
	
	#pragma mark -
	#pragma mark ¥ AEDesc ¥
	
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
	
	#pragma mark -
	#pragma mark ¥ Coercions ¥
	
	struct AECoerceDescUPP_Details: Basic_UPP_Details< ::AECoerceDescUPP,
	                                                   ::AECoerceDescProcPtr,
	                                                   ::NewAECoerceDescUPP,
	                                                   ::DisposeAECoerceDescUPP,
	                                                   ::InvokeAECoerceDescUPP >
	{};
	
	struct AECoercePtrUPP_Details: Basic_UPP_Details< ::AECoercePtrUPP,
	                                                  ::AECoercePtrProcPtr,
	                                                  ::NewAECoercePtrUPP,
	                                                  ::DisposeAECoercePtrUPP,
	                                                  ::InvokeAECoercePtrUPP >
	{};
	
	typedef UPP< AECoerceDescUPP_Details > AECoerceDescUPP;
	typedef UPP< AECoercePtrUPP_Details  > AECoercePtrUPP;
	
	inline Owned< AECoerceDescUPP > NewAECoerceDescUPP( ::AECoerceDescProcPtr p )
	{
		return NewUPP< AECoerceDescUPP >( p );
	}
	
	inline Owned< AECoercePtrUPP > NewAECoercePtrUPP( ::AECoercePtrProcPtr p )
	{
		return NewUPP< AECoercePtrUPP >( p );
	}
	
	inline void DisposeAECoerceDescUPP( Owned< AECoerceDescUPP > )  {}
	inline void DisposeAECoercePtrUPP ( Owned< AECoercePtrUPP  > )  {}
	
	inline void InvokeAECoerceDescUPP( const AEDesc& fromDesc,
	                                   DescType toType,
	                                   RefCon handlerRefCon,
	                                   AEDesc& toDesc, 
	                                   AECoerceDescUPP userUPP )
	{
		ThrowOSStatus( userUPP( &fromDesc, toType, handlerRefCon, &toDesc ) );
	}
	
	inline void InvokeAECoercePtrUPP( DescType typeCode,
	                                  const void* dataPtr,
	                                  std::size_t dataSize,
	                                  DescType toType,
	                                  RefCon handlerRefCon,
	                                  AEDesc& toDesc, 
	                                  AECoercePtrUPP userUPP )
	{
		ThrowOSStatus( userUPP( typeCode, dataPtr, dataSize, toType, handlerRefCon, &toDesc ) );
	}
	
	struct AECoercionHandlerUPP : AECoerceDescUPP
	{
		AECoercionHandlerUPP()  {}
		AECoercionHandlerUPP( ::AECoerceDescUPP p ) : AECoerceDescUPP( p )  {}
		AECoercionHandlerUPP( ::AECoercePtrUPP p )  : AECoerceDescUPP( reinterpret_cast< ::AECoerceDescUPP >( p ) )  {}
	};
	
	struct AECoercionHandler
	{
		DescType               fromType;
		DescType               toType;
		AECoercionHandlerUPP   handler;
		RefCon                 handlerRefCon;
		bool                   fromTypeIsDesc;
		bool                   isSysHandler;
		
		AECoercionHandler();
		
		AECoercionHandler( DescType             fromType,
		                   DescType             toType,
		                   AECoercionHandlerUPP handler,
		                   RefCon               handlerRefCon,
		                   bool                 fromTypeIsDesc,
		                   bool                 isSysHandler )
		: 
			fromType( fromType ),
			toType( toType ),
			handler( handler ),
			handlerRefCon( handlerRefCon ),
			fromTypeIsDesc( fromTypeIsDesc ),
			isSysHandler( isSysHandler )
		{}
		
		AECoercionHandler( DescType        fromType,
		                   DescType        toType,
		                   AECoerceDescUPP handler,
		                   RefCon          handlerRefCon,
		                   bool            isSysHandler )
		: 
			fromType( fromType ),
			toType( toType ),
			handler( handler ),
			handlerRefCon( handlerRefCon ),
			fromTypeIsDesc( true ),
			isSysHandler( isSysHandler )
		{}
		
		AECoercionHandler( DescType       fromType,
		                   DescType       toType,
		                   AECoercePtrUPP handler,
		                   RefCon         handlerRefCon,
		                   bool           isSysHandler )
		: 
			fromType( fromType ),
			toType( toType ),
			handler( handler ),
			handlerRefCon( handlerRefCon ),
			fromTypeIsDesc( false ),
			isSysHandler( isSysHandler )
		{}
	};
	
	bool operator==( const AECoercionHandler& a, const AECoercionHandler& b );
	
	inline bool operator!=( const AECoercionHandler& a, const AECoercionHandler& b )
	{
		return !( a == b );
	}
	
	template <>
	struct Disposer< AECoercionHandler > : public std::unary_function< AECoercionHandler, void >,
	                                       private DefaultDestructionOSStatusPolicy
	{
		void operator()( const AECoercionHandler& installation ) const
		{
			DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus
			(
				::AERemoveCoercionHandler( installation.fromType,
				                           installation.toType,
				                           installation.handler,
				                           installation.isSysHandler )
			);
		}
	};
	
	typedef Owned< AEDesc > ( *AECoerceDescProcPtr )( const AEDesc& fromDesc, DescType toType, RefCon refCon );
	typedef Owned< AEDesc > ( *AECoercePtrProcPtr )( DescType typeCode, const void* dataPtr, std::size_t datasize, DescType toType, RefCon refCon );
	
	template < AECoerceDescProcPtr handler >
	struct Adapt_AECoerceDesc
	{
		static pascal OSErr ToCallback( const AEDesc* fromDesc, ::DescType toType, long refCon, AEDesc* result )
		{
			try
			{
				*result = handler( *fromDesc, toType, refCon ).Release();
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			return noErr;
		}
	};
	
	template < AECoercePtrProcPtr handler >
	struct Adapt_AECoercePtr
	{
		static pascal OSErr ToCallback( ::DescType typeCode, const void* dataPtr, ::Size datasize, ::DescType toType, long refCon, AEDesc* result )
		{
			try
			{
				*result = handler( typeCode, dataPtr, dataSize, toType, refCon ).Release();
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			return noErr;
		}
	};
	
	// 388
	Owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall );
	
	inline Owned< AECoercionHandler > AEInstallCoercionHandler
	(
		DescType fromType, 
		DescType toType, 
		AECoerceDescUPP handler, 
		RefCon handlerRefCon = RefCon(), 
		Boolean isSysHandler = false
	)
	{
		return AEInstallCoercionHandler
		(
			AECoercionHandler( fromType, toType, handler, handlerRefCon, isSysHandler )
		);
	}
	
	inline Owned< AECoercionHandler > AEInstallCoercionHandler
	(
		DescType fromType, 
		DescType toType, 
		AECoercePtrUPP handler, 
		RefCon handlerRefCon = RefCon(), 
		Boolean isSysHandler = false
	)
	{
		return AEInstallCoercionHandler
		(
			AECoercionHandler( fromType, toType, handler, handlerRefCon, isSysHandler )
		);
	}
	
	template < typename AECoerceDescUPP::ProcPtr handler >
	Owned< AECoercionHandler > AEInstallCoercionHandler
	(
		DescType fromType, 
		DescType toType, 
		RefCon handlerRefCon = RefCon(), 
		Boolean isSysHandler = false
	)
	{
		return AEInstallCoercionHandler
		(
			AECoercionHandler
			(
				fromType, 
				toType, 
				StaticUPP< AECoerceDescUPP, handler >(), 
				handlerRefCon, 
				isSysHandler
			)
		);
	}
	
	template < typename AECoercePtrUPP::ProcPtr handler >
	Owned< AECoercionHandler > AEInstallCoercionHandler
	(
		DescType fromType, 
		DescType toType, 
		RefCon handlerRefCon = RefCon(), 
		Boolean isSysHandler = false
	)
	{
		return AEInstallCoercionHandler
		(
			AECoercionHandler
			(
				fromType, 
				toType, 
				StaticUPP< AECoercePtrUPP, handler >(), 
				handlerRefCon, 
				isSysHandler
			)
		);
	}
	
	template < AECoerceDescProcPtr handler >
	Owned< AECoercionHandler > AEInstallCoercionHandler
	(
		DescType fromType, 
		DescType toType, 
		RefCon handlerRefCon = RefCon(), 
		Boolean isSysHandler = false
	)
	{
		return AEInstallCoercionHandler< Adapt_AECoerceDesc< handler >::ToCallback >
		(
			fromType, 
			toType, 
			handlerRefCon, 
			isSysHandler
		);
	}
	
	template < AECoercePtrProcPtr handler >
	Owned< AECoercionHandler > AEInstallCoercionHandler
	(
		DescType fromType, 
		DescType toType, 
		RefCon handlerRefCon = RefCon(), 
		Boolean isSysHandler = false
	)
	{
		return AEInstallCoercionHandler< Adapt_AECoercePtr< handler >::ToCallback >
		(
			fromType, 
			toType, 
			handlerRefCon, 
			isSysHandler
		);
	}
	
	// 406
	inline void AERemoveCoercionHandler( Owned< AECoercionHandler > )  {}
	
	typedef AECoercionHandler AEGetCoercionHandler_Result;
	AECoercionHandler AEGetCoercionHandler
	(
		DescType fromType,
		DescType toType,
		bool isSysHandler
	);
	
	// 444
	Owned< AEDesc > AECoercePtr(DescType typeCode, const void* dataPtr, Size dataSize, DescType toType);
	
	// 461
	Owned< AEDesc > AECoerceDesc(const AEDesc& desc, DescType toType);
	
	#pragma mark -
	#pragma mark ¥ AEDescs ¥
	
	struct AEDesc_Info
	{
		DescType typeCode;
		Size dataSize;
		
		operator Size() const  { return dataSize; }
	};
	
	typedef AEDesc_Info 
		AESizeOfNthItem_Result, 
		AESizeOfKeyDesc_Result, 
		AEGetKeyPtr_Result, 
		AESizeOfParam_Result, 
		AESizeOfAttribute_Result, 
		AEGetParamPtr_Result, 
		AEGetAttributePtr_Result;

	// 484
	inline Owned< AEDesc > AEInitializeDesc()
	{
		return Owned< AEDesc >::Seize( Make< AEDesc >() );
	}
	
	Owned< AEDesc > AECreateDesc(DescType typeCode, const void* dataPtr, Size dataSize);
	Owned< AEDesc > AECreateDesc(DescType typeCode, ::Handle handle);
	Owned< AEDesc > AECreateDesc(DescType typeCode, Owned< Handle > handle);
	
	template < class T >
	Owned< AEDesc > AECreateDesc( DescType typeCode, Owned< T**, Disposer< Handle > > handle )
	{
		return AECreateDesc( typeCode, Owned< Handle >( handle ) );
	}
	
	Owned< AEDesc > AECreateDesc(DescType typeCode, Owned< AEDesc > desc);
	
	template < class Data >
	Owned< AEDesc > AECreateDesc(DescType typeCode, Data** handle)
	{
		return AECreateDesc( typeCode, reinterpret_cast< ::Handle >( handle ) );
	}
	
	
	inline void AEDisposeDesc( Owned< AEDesc > )  {}
	
	Owned< AEDesc > AEDuplicateDesc( const AEDesc& desc );
	
	#pragma mark -
	#pragma mark ¥ AEDescLists ¥
	
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
	
	struct GetNthPtr_Result
	{
		AEKeyword keyword;
		DescType typeCode;
		Size actualSize;
		
		GetNthPtr_Result() : keyword(), typeCode(), actualSize() {}
		GetNthPtr_Result(AEKeyword keyword, DescType typeCode, ::Size actualSize) 
		  : keyword(keyword), typeCode(typeCode), actualSize(actualSize)  {}
	};
	
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
		DescType desiredType = TypeWildCard(), 
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
	
	#pragma mark -
	#pragma mark ¥ AERecords ¥
	
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
	
	#pragma mark -
	#pragma mark ¥ AppleEvents ¥
	
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
	
	#pragma mark -
	#pragma mark ¥ Opaque data ¥
	
	void AEGetDescData( const AEDesc& desc, void* dataPtr, std::size_t maximumSize );
	
	inline std::size_t AEGetDescDataSize( const AEDesc& desc )
	{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		
		return ::AEGetDescDataSize( &desc );
		
	#else
		
		return GetHandleSize( desc.dataHandle );
		
	#endif
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
	
	
	#pragma mark -
	#pragma mark ¥ AEEventHandlerUPP ¥
	
   struct AEEventHandlerUPP_Details: Basic_UPP_Details< ::AEEventHandlerUPP,
                                                        ::AEEventHandlerProcPtr,
                                                        ::NewAEEventHandlerUPP,
                                                        ::DisposeAEEventHandlerUPP,
                                                        ::InvokeAEEventHandlerUPP >
     {};
   
   typedef UPP< AEEventHandlerUPP_Details > AEEventHandlerUPP;
   
   //using ::AEEventHandlerProcPtr;
   
   inline Owned< AEEventHandlerUPP > NewAEEventHandlerUPP( ::AEEventHandlerProcPtr p )
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
	
	#pragma mark -
	#pragma mark ¥ Powered by DescType_Traits ¥
	
	template < ::DescType type >
	Owned< AEDesc > AECoercePtr(typename DescType_Traits< type >::Parameter data, DescType toType)
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		Owned< AEDesc > result = AECoercePtr(
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
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
			DescType( type ), 
			Traits::OutputBufferStart( buffer ), 
			Traits::OutputBufferLength( buffer ), 
			result
		);
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	
}

#endif
