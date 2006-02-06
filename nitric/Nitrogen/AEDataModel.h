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

#ifndef NUCLEUS_SELECTORTYPE_H
#include "Nucleus/SelectorType.h"
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
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
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

inline OSErr AEPutKeyPtr( AppleEvent*  theAppleEvent,
                          AEKeyword    theAEKeyword,
                          DescType     typeCode,
                          const void*  dataPtr,
                          Size         dataSize )
{
	return AEPutParamPtr( theAppleEvent,
	                      theAEKeyword,
	                      typeCode,
	                      dataPtr,
	                      dataSize );
}

#endif

#ifdef AEPutKeyDesc
#undef AEPutKeyDesc

inline OSErr AEPutKeyDesc( AppleEvent*    theAppleEvent,
                           AEKeyword      theAEKeyword,
                           const AEDesc*  theAEDesc )
{
	return AEPutParamDesc( theAppleEvent, theAEKeyword, theAEDesc );
}

#endif

#ifdef AEGetKeyPtr
#undef AEGetKeyPtr

inline OSErr AEGetKeyPtr( const AppleEvent*  theAppleEvent,
                          AEKeyword          theAEKeyword,
                          DescType           desiredType,
                          DescType*          typeCode,
                          void*              dataPtr,
                          Size               maximumSize,
                          Size*              actualSize )
{
	return AEGetParamPtr( theAppleEvent,
	                      theAEKeyword,
	                      desiredType,
	                      typeCode,
	                      dataPtr,
	                      maximumSize,
	                      actualSize );
}

#endif

#ifdef AEGetKeyDesc
#undef AEGetKeyDesc

inline OSErr AEGetKeyDesc( const AppleEvent*  theAppleEvent,
                           AEKeyword          theAEKeyword,
                           DescType           desiredType,
                           AEDesc*            result )
{
	return AEGetParamDesc( theAppleEvent,
	                       theAEKeyword,
	                       desiredType,
	                       result );
}

#endif

#ifdef AESizeOfKeyDesc
#undef AESizeOfKeyDesc

inline OSErr AESizeOfKeyDesc( const AppleEvent*  theAppleEvent,
                              AEKeyword          theAEKeyword,
                              DescType*          typeCode,
                              Size*              dataSize )
{
	return AESizeOfParam( theAppleEvent,
	                      theAEKeyword,
	                      typeCode,
	                      dataSize );
}

#endif

#ifdef AEDeleteKeyDesc
#undef AEDeleteKeyDesc

inline OSErr AEDeleteKeyDesc( AppleEvent*  theAppleEvent,
                              AEKeyword    theAEKeyword )
{
	return AEDeleteParam( theAppleEvent, theAEKeyword );
}

#endif


namespace Nitrogen
{
	
	void RegisterAppleEventManagerErrors();
	
	#pragma mark -
	#pragma mark ¥ DescType ¥
	
	class DescType_Tag {};
	typedef Nucleus::SelectorType< DescType_Tag, ::DescType > DescType;
	
	typedef DescType AEObjectClass, AEPropertyID;
	
	class AEKeyword_Tag {};
	typedef Nucleus::SelectorType< AEKeyword_Tag, ::AEKeyword > AEKeyword;
	
	// Constants used creating an AppleEvent
	
	class AEReturnID_Tag {};
	typedef Nucleus::IDType< AEReturnID_Tag, ::AEReturnID > AEReturnID;
	
	inline AEReturnID AutoGenerateReturnID()  { return AEReturnID::Make( kAutoGenerateReturnID ); }
	
	class AETransactionID_Tag {};
	typedef Nucleus::IDType< AETransactionID_Tag, ::AETransactionID > AETransactionID;
	
	inline AETransactionID AnyTransactionID()  { return AETransactionID::Make( kAnyTransactionID ); }
	
	struct AESendPriority_Tag  {};
	typedef Nucleus::FlagType< AESendPriority_Tag, ::AESendPriority, kAENormalPriority > AESendPriority;
	
	struct AESendMode_Tag  {};
	typedef Nucleus::FlagType< AESendMode_Tag, ::AESendMode > AESendMode;
	
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
      typedef Nucleus::Owned< Ownable > Result;
      typedef Ownable Parameter;
      
      typedef Ownable        InputBuffer;
      typedef const Ownable& OutputBuffer;

      static const bool inputHasVariableLength = false;
      static void SetInputBufferLength( InputBuffer&, std::size_t )          {}

      static void *      InputBufferStart(  InputBuffer& buffer )            { return &buffer; }
      static std::size_t InputBufferLength( InputBuffer& buffer )            { return sizeof( buffer ); }
      static Result   ProcessInputBuffer( InputBuffer& buffer )              { return Nucleus::Owned< Ownable >::Seize( buffer ); }

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
		
		typedef Nucleus::Owned< HandleType > Result;
		typedef HandleType Parameter;
		
		typedef Nucleus::Owned< Handle > InputBuffer;
		
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

      static const void *OutputBufferStart(  OutputBuffer &buffer )          { return &buffer; }
      static std::size_t OutputBufferLength( OutputBuffer &buffer )          { return sizeof( buffer ); }
      static OutputBuffer PrepareOutputBuffer( Parameter output )           { return DoubleToFixed( output ); }
      static void ReleaseOutputBuffer( OutputBuffer )                       {}
     };

   class AEEnumeratedTag {};
   typedef Nucleus::SelectorType< AEEnumeratedTag, UInt32 > AEEnumerated;
   typedef AEEnumerated AEEnumeration;
   
   class AEEventClassTag {};
   typedef Nucleus::SelectorType< AEEventClassTag, ::AEEventClass > AEEventClass;

   class AEEventIDTag {};
   typedef Nucleus::SelectorType< AEEventIDTag, ::AEEventID > AEEventID;
   
   template<> struct DescType_Traits< typeNull >                   { typedef void Result; };
   
   template<> struct DescType_Traits< typeBoolean >                : Converting_DescType_Traits< bool, ::Boolean >        {};
   template<> struct DescType_Traits< typeSInt16 >                 : POD_DescType_Traits< SInt16 >                        {};
   template<> struct DescType_Traits< typeSInt32 >                 : POD_DescType_Traits< SInt32 >                        {};
   template<> struct DescType_Traits< typeUInt32 >                 : POD_DescType_Traits< UInt32 >                        {};
   template<> struct DescType_Traits< typeSInt64 >                 : POD_DescType_Traits< SInt64 >                        {};
   template<> struct DescType_Traits< typeIEEE32BitFloatingPoint > : POD_DescType_Traits< float >                         {};
   template<> struct DescType_Traits< typeIEEE64BitFloatingPoint > : POD_DescType_Traits< double >                        {};
   template<> struct DescType_Traits< type128BitFloatingPoint >    : POD_DescType_Traits< long double >                   {};

	inline std::size_t SizeOf_AppParameters( const AppParameters& appParameters )
	{
		return sizeof (AppParameters) + appParameters.messageLength;
	}
	
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
   
	
	#pragma mark -
	#pragma mark ¥ AEDesc ¥
	
	using ::AEDesc;
	using ::AEDescList;
    using ::AEKeyDesc;
	using ::AEAddressDesc;
	using ::AERecord;
	using ::AppleEvent;
	
	typedef AEDesc     AEToken;
	typedef AEDescList AETokenList;
	typedef AERecord   AEObjectSpecifier;
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::AEDesc > : public std::unary_function< Nitrogen::AEDesc, void >,
	                                      private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		// parameter can't be const
		void operator()( Nitrogen::AEDesc desc ) const
		{
			// AEDisposeDesc() is documented as only ever returning noErr,
			// but we check anyway to be future-proof.
			
			Nucleus::OnlyOnce< Nitrogen::RegisterAppleEventManagerErrors >();
			HandleDestructionOSStatus( ::AEDisposeDesc( &desc ) );
		}
	};

	template <>
	struct LivelinessTraits< Nitrogen::AEDesc, Disposer< Nitrogen::AEDesc > >   { typedef SeizedValuesAreLive LivelinessTest; };
	
	template <>
	struct Maker< Nitrogen::AEDesc >
	{
		Nitrogen::AEDesc operator()() const
		{
			Nitrogen::AEDesc result = { typeNull, NULL };
			return result;
		}
	};
	
	template <>
	struct Disposer< Nitrogen::AEKeyDesc > : public std::unary_function< Nitrogen::AEKeyDesc, void >,
	                                         private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		// parameter can't be const
		void operator()( Nitrogen::AEKeyDesc keyDesc ) const
		{
			Disposer< Nitrogen::AEDesc >()( keyDesc.descContent );
		}
	};
	
	template <>
	struct LivelinessTraits< Nitrogen::AEKeyDesc, Disposer< Nitrogen::AEKeyDesc > >
	{
		typedef SeizedValuesAreLive LivelinessTest;
	};
	
	template <>
	struct Maker< Nitrogen::AEKeyDesc >
	{
		Nitrogen::AEKeyDesc operator()() const
		{
			Nitrogen::AEKeyDesc result = { Nitrogen::AEKeyword(), Make< Nitrogen::AEDesc >() };
			return result;
		}
	};
  }

namespace Nitrogen
  {	
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
	
	inline Nucleus::Owned< AECoerceDescUPP > NewAECoerceDescUPP( ::AECoerceDescProcPtr p )
	{
		return NewUPP< AECoerceDescUPP >( p );
	}
	
	inline Nucleus::Owned< AECoercePtrUPP > NewAECoercePtrUPP( ::AECoercePtrProcPtr p )
	{
		return NewUPP< AECoercePtrUPP >( p );
	}
	
	inline void DisposeAECoerceDescUPP( Nucleus::Owned< AECoerceDescUPP > )  {}
	inline void DisposeAECoercePtrUPP ( Nucleus::Owned< AECoercePtrUPP  > )  {}
	
	inline void InvokeAECoerceDescUPP( const AEDesc&    fromDesc,
	                                   DescType         toType,
	                                   RefCon           handlerRefCon,
	                                   AEDesc&          toDesc,
	                                   AECoerceDescUPP  userUPP )
	{
		ThrowOSStatus( userUPP( &fromDesc,
		                        toType,
		                        handlerRefCon,
		                        &toDesc ) );
	}
	
	inline void InvokeAECoercePtrUPP( DescType        typeCode,
	                                  const void*     dataPtr,
	                                  std::size_t     dataSize,
	                                  DescType        toType,
	                                  RefCon          handlerRefCon,
	                                  AEDesc&         toDesc,
	                                  AECoercePtrUPP  userUPP )
	{
		ThrowOSStatus( userUPP( typeCode,
		                        dataPtr,
		                        dataSize,
		                        toType,
		                        handlerRefCon,
		                        &toDesc ) );
	}
	
	struct AECoercionHandlerUPP : AECoerceDescUPP
	{
		AECoercionHandlerUPP()  {}
		AECoercionHandlerUPP( ::AECoerceDescUPP p ) : AECoerceDescUPP( p )  {}
	
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM  ||  OPAQUE_UPP_TYPES
		
		AECoercionHandlerUPP( ::AECoercePtrUPP p )
		:
			AECoerceDescUPP( reinterpret_cast< ::AECoerceDescUPP >( p ) )
		{}
		
	#endif
	};
	
	struct AECoercionHandler
	{
		DescType              fromType;
		DescType              toType;
		AECoercionHandlerUPP  handler;
		RefCon                handlerRefCon;
		bool                  fromTypeIsDesc;
		bool                  isSysHandler;
		
		AECoercionHandler();
		
		AECoercionHandler( DescType              fromType,
		                   DescType              toType,
		                   AECoercionHandlerUPP  handler,
		                   RefCon                handlerRefCon,
		                   bool                  fromTypeIsDesc,
		                   bool                  isSysHandler )
		:
			fromType      ( fromType       ),
			toType        ( toType         ),
			handler       ( handler        ),
			handlerRefCon ( handlerRefCon  ),
			fromTypeIsDesc( fromTypeIsDesc ),
			isSysHandler  ( isSysHandler   )
		{}
		
		AECoercionHandler( DescType         fromType,
		                   DescType         toType,
		                   AECoerceDescUPP  handler,
		                   RefCon           handlerRefCon,
		                   bool             isSysHandler )
		:
			fromType      ( fromType      ),
			toType        ( toType        ),
			handler       ( handler       ),
			handlerRefCon ( handlerRefCon ),
			fromTypeIsDesc( true          ),
			isSysHandler  ( isSysHandler  )
		{}
		
		AECoercionHandler( DescType        fromType,
		                   DescType        toType,
		                   AECoercePtrUPP  handler,
		                   RefCon          handlerRefCon,
		                   bool            isSysHandler )
		:
			fromType      ( fromType      ),
			toType        ( toType        ),
			handler       ( handler       ),
			handlerRefCon ( handlerRefCon ),
			fromTypeIsDesc( false         ),
			isSysHandler  ( isSysHandler  )
		{}
	};
	
	bool operator==( const AECoercionHandler& a, const AECoercionHandler& b );
	
	inline bool operator!=( const AECoercionHandler& a, const AECoercionHandler& b )
	{
		return !( a == b );
	}
	
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::AECoercionHandler > : public std::unary_function< Nitrogen::AECoercionHandler, void >,
												     private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( const Nitrogen::AECoercionHandler& installation ) const
		{
			HandleDestructionOSStatus( ::AERemoveCoercionHandler( installation.fromType,
			                                                      installation.toType,
			                                                      installation.handler,
			                                                      installation.isSysHandler ) );
		}
	};
  }

namespace Nitrogen
  {
	typedef Nucleus::Owned< AEDesc > ( *AECoerceDescProcPtr )( const AEDesc&  fromDesc,
	                                                  DescType       toType,
	                                                  RefCon         refCon );
	
	typedef Nucleus::Owned< AEDesc > ( *AECoercePtrProcPtr )( DescType     typeCode,
	                                                 const void*  dataPtr,
	                                                 std::size_t  datasize,
	                                                 DescType     toType,
	                                                 RefCon       refCon );
	
	template < AECoerceDescProcPtr handler >
	struct Adapt_AECoerceDesc
	{
		static pascal OSErr ToCallback( const AEDesc*  fromDesc,
		                                ::DescType     toType,
		                                long           refCon,
		                                AEDesc*        result )
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
		static pascal OSErr ToCallback( ::DescType   typeCode,
		                                const void*  dataPtr,
		                                ::Size       dataSize,
		                                ::DescType   toType,
		                                long         refCon,
		                                AEDesc*      result )
		{
			try
			{
				*result = handler( typeCode,
				                   dataPtr,
				                   dataSize,
				                   toType,
				                   refCon ).Release();
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			
			return noErr;
		}
	};
	
	// 388
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall );
	
	inline Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType fromType,
	                                                            DescType toType,
	                                                            AECoerceDescUPP handler,
	                                                            RefCon handlerRefCon = RefCon(),
	                                                            Boolean isSysHandler = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	inline Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType        fromType,
	                                                            DescType        toType,
	                                                            AECoercePtrUPP  handler,
	                                                            RefCon          handlerRefCon = RefCon(),
	                                                            Boolean         isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoerceDescUPP::ProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                     DescType  toType,
	                                                     RefCon    handlerRefCon = RefCon(),
	                                                     Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoerceDescUPP, handler >(),
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoercePtrUPP::ProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                     DescType  toType,
	                                                     RefCon    handlerRefCon = RefCon(),
	                                                     Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoercePtrUPP, handler >(),
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < AECoerceDescProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                     DescType  toType,
	                                                     RefCon    handlerRefCon = RefCon(),
	                                                     Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler< Adapt_AECoerceDesc< handler >::ToCallback >( fromType,
		                                                                              toType,
		                                                                              handlerRefCon,
		                                                                              isSysHandler );
	}
	
	template < AECoercePtrProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                     DescType  toType,
	                                                     RefCon    handlerRefCon = RefCon(),
	                                                     Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler< Adapt_AECoercePtr< handler >::ToCallback >( fromType,
		                                                                             toType,
		                                                                             handlerRefCon,
		                                                                             isSysHandler );
	}
	
	// 406
	inline void AERemoveCoercionHandler( Nucleus::Owned< AECoercionHandler > )  {}
	
	typedef AECoercionHandler AEGetCoercionHandler_Result;
	
	AECoercionHandler AEGetCoercionHandler( DescType  fromType,
	                                        DescType  toType,
	                                        bool      isSysHandler );
	
	// 444
	Nucleus::Owned< AEDesc > AECoercePtr( DescType     typeCode,
	                             const void*  dataPtr,
	                             Size         dataSize,
	                             DescType     toType );
	
	// 461
	Nucleus::Owned< AEDesc > AECoerceDesc( const AEDesc& desc, DescType toType );
	
	#pragma mark -
	#pragma mark ¥ AEDescs ¥
	
	struct AEDesc_Info
	{
		DescType typeCode;
		Size dataSize;
		
		operator Size() const  { return dataSize; }
	};
	
	typedef AEDesc_Info AESizeOfNthItem_Result,
	                    AESizeOfKeyDesc_Result,
	                    AEGetKeyPtr_Result,
	                    AESizeOfParam_Result,
	                    AESizeOfAttribute_Result,
	                    AEGetParamPtr_Result,
	                    AEGetAttributePtr_Result;

	// 484
	inline Nucleus::Owned< AEDesc > AEInitializeDesc()
	{
		return Nucleus::Owned< AEDesc >::Seize( Nucleus::Make< AEDesc >() );
	}
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType     typeCode,
	                              const void*  dataPtr,
	                              Size         dataSize );
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType  typeCode,
	                              Handle    handle );
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType         typeCode,
	                              Nucleus::Owned< Handle >  handle );
	
	template < class T >
	Nucleus::Owned< AEDesc > AECreateDesc( DescType typeCode, T** handle )
	{
		return AECreateDesc( typeCode, Handle( handle ) );
	}
	
	template < class T >
	Nucleus::Owned< AEDesc > AECreateDesc( DescType                          typeCode,
	                              Nucleus::Owned< T**, Nucleus::Disposer< Handle > >  handle )
	{
		return AECreateDesc( typeCode, Nucleus::Owned< Handle >( handle ) );
	}
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType typeCode, Nucleus::Owned< AEDesc > desc );
	
	
	inline void AEDisposeDesc( Nucleus::Owned< AEDesc > )  {}
	
	Nucleus::Owned< AEDesc > AEDuplicateDesc( const AEDesc& desc );
	
	#pragma mark -
	#pragma mark ¥ AEDescLists ¥
	
	Nucleus::Owned< AEDesc > AECreateList( bool isRecord = false );
	
	Nucleus::Owned< AEDesc > AECreateList( const void*  factoringPtr,
	                              std::size_t  factoredSize,
	                              bool         isRecord );
	
	inline Nucleus::Owned< AEDesc > AECreateRecord()  { return AECreateList( true ); }
	
	template < bool isRecord >
	Nucleus::Owned< AEDesc > AECreateList()  { return AECreateList( isRecord ); }
	
	template < bool isRecord >
	Nucleus::Owned< AEDesc > AECreateList( DescType typeCode )
	{
		return AECreateDesc( typeCode, AECreateList< isRecord >() );
	}
	
	long AECountItems( const AEDesc& desc );
	
	void AEPutPtr( AEDescList&  list,
	               long         index,
	               DescType     type,
	               const void*  dataPtr,
	               Size         dataSize );
	
	void AEPutPtr( Nucleus::Owned< AEDescList >&  list,
	               long                  index,
	               DescType              type,
	               const void*           dataPtr,
	               Size                  dataSize );
	
	void AEPutDesc( AEDescList&    list,
	                long           index,
	                const AEDesc&  desc );
	
	void AEPutDesc( Nucleus::Owned< AEDescList >&  list,
	                long                  index,
	                const AEDesc&         desc );
	
	struct GetNthPtr_Result
	{
		AEKeyword keyword;
		DescType typeCode;
		Size actualSize;
	};
	
	GetNthPtr_Result AEGetNthPtr( const AEDesc&  listDesc,
	                              long           index,
	                              DescType       desiredType,
	                              void*          dataPtr,
	                              Size           maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetNthDesc( const AEDesc&  listDesc,
	                              long           index,
	                              DescType       desiredType = typeWildCard,
	                              ::AEKeyword*   keyword     = NULL );
	
	Nucleus::Owned< AEDesc > AEGetNthDesc( const AEDesc&  listDesc,
	                              long           index,
	                              ::AEKeyword*   keyword );
	
	Size AESizeOfNthItem( const AEDescList& list, long index );
	
	void AEDeleteItem(        AEDescList  & listDesc, long index );
	void AEDeleteItem( Nucleus::Owned< AEDescList >& listDesc, long index );
	
	#pragma mark -
	#pragma mark ¥ AERecords ¥
	
	inline bool AECheckIsRecord( const AEDesc& theDesc )
	{
		return ::AECheckIsRecord( &theDesc );
	}
	
	void AEPutKeyPtr( AERecord&    record,
	                  AEKeyword    keyword,
	                  DescType     typeCode,
	                  const void*  dataPtr,
	                  std::size_t  dataSize );
	
	void AEPutKeyPtr( Nucleus::Owned< AERecord >&  record,
	                  AEKeyword           keyword,
	                  DescType            typeCode,
	                  const void*         dataPtr,
	                  std::size_t         dataSize );
	
	void AEPutKeyDesc( AERecord&      record,
	                   AEKeyword      keyword,
	                   const AEDesc&  desc );
	
	void AEPutKeyDesc( AERecord&         record,
	                   const AEKeyDesc&  keyDesc );
	
	void AEPutKeyDesc( Nucleus::Owned< AERecord >&  record,
	                   AEKeyword           keyword,
	                   const AEDesc&       desc );
	
	void AEPutKeyDesc( Nucleus::Owned< AERecord >&  record,
	                   const AEKeyDesc&    keyDesc );
	
	AEGetKeyPtr_Result AEGetKeyPtr( const AERecord&  record,
	                                AEKeyword        keyword,
	                                DescType         desiredType,
	                                void*            dataPtr,
	                                std::size_t      maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetKeyDesc( const AERecord&  record,
	                              AEKeyword        keyword,
	                              DescType         desiredType );
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc( const AERecord&  record,
	                                        AEKeyword        keyword );
	
	void AEDeleteKeyDesc(        AERecord  & record, AEKeyword keyword );
	void AEDeleteKeyDesc( Nucleus::Owned< AERecord >& record, AEKeyword keyword );
	
	#pragma mark -
	#pragma mark ¥ AppleEvents ¥
	
	Nucleus::Owned< AppleEvent > AECreateAppleEvent( AEEventClass          eventClass,
	                                        AEEventID             eventID,
	                                        const AEAddressDesc&  target,
	                                        AEReturnID            returnID      = AEReturnID(),
	                                        AETransactionID       transactionID = AETransactionID() );
	
	void AEPutParamPtr( AppleEvent&  appleEvent,
	                    AEKeyword    keyword,
	                    DescType     typeCode,
	                    const void*  dataPtr,
	                    std::size_t  dataSize );
	
	void AEPutParamPtr( Nucleus::Owned< AppleEvent >&  appleEvent,
	                    AEKeyword             keyword,
	                    DescType              typeCode,
	                    const void*           dataPtr,
	                    std::size_t           dataSize );
	
	void AEPutParamDesc( AppleEvent&    appleEvent,
	                     AEKeyword      keyword,
	                     const AEDesc&  desc );
	
	void AEPutParamDesc( Nucleus::Owned< AppleEvent >&  appleEvent,
	                     AEKeyword             keyword,
	                     const AEDesc&         desc );
	
	AEGetParamPtr_Result AEGetParamPtr( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword,
	                                    DescType           desiredType,
	                                    void*              dataPtr,
	                                    std::size_t        maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetParamDesc( const AppleEvent&  appleEvent,
	                                AEKeyword          keyword,
	                                DescType           desiredType );
	
	AESizeOfParam_Result AESizeOfParam( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword );
	
	void AEDeleteParam(        AppleEvent  & appleEvent, AEKeyword keyword );
	void AEDeleteParam( Nucleus::Owned< AppleEvent >& appleEvent, AEKeyword keyword );
	
	void AEPutAttributePtr( AppleEvent&  appleEvent,
	                        AEKeyword    keyword,
	                        DescType     typeCode,
	                        const void*  dataPtr,
	                        std::size_t  dataSize );
	
	void AEPutAttributePtr( Nucleus::Owned< AppleEvent >&  appleEvent,
	                        AEKeyword             keyword,
	                        DescType              typeCode,
	                        const void*           dataPtr,
	                        std::size_t           dataSize );
	
	void AEPutAttributeDesc( AppleEvent&    appleEvent,
	                         AEKeyword      keyword,
	                         const AEDesc&  desc );
	
	void AEPutAttributeDesc( Nucleus::Owned< AppleEvent >&  appleEvent,
	                         AEKeyword             keyword,
	                         const AEDesc&         desc );
	
	AEGetAttributePtr_Result AEGetAttributePtr( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword,
	                                            DescType           desiredType,
	                                            void*              dataPtr,
	                                            std::size_t        maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetAttributeDesc( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword,
	                                    DescType           desiredType );
	
	AESizeOfAttribute_Result AESizeOfAttribute( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword );
	
	#pragma mark -
	#pragma mark ¥ Opaque data ¥
	
	void AEGetDescData( const AEDesc&  desc,
	                    void*          dataPtr,
	                    std::size_t    maximumSize );
	
	inline std::size_t AEGetDescDataSize( const AEDesc& desc )
	{
	#if ACCESSOR_CALLS_ARE_FUNCTIONS
		
		return ::AEGetDescDataSize( &desc );
		
	#else
		
		return GetHandleSize( desc.dataHandle );
		
	#endif
	}
	
	void AEReplaceDescData( DescType     typeCode,
	                        const void*  dataPtr,
	                        std::size_t  dataSize,
	                        AEDesc&      result );
	
	void AEReplaceDescData( DescType          typeCode,
	                        const void*       dataPtr,
	                        std::size_t       dataSize,
	                        Nucleus::Owned< AEDesc >&  result );
	
	
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
   
   inline Nucleus::Owned< AEEventHandlerUPP > NewAEEventHandlerUPP( ::AEEventHandlerProcPtr p )
     {
      return NewUPP< AEEventHandlerUPP >( p );
     }

   inline void DisposeAEEventHandlerUPP( Nucleus::Owned< AEEventHandlerUPP > )
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
	Nucleus::Owned< AEDesc > AECoercePtr( typename DescType_Traits< type >::Parameter  data,
	                             DescType                                     toType )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		Nucleus::Owned< AEDesc > result = AECoercePtr( DescType( type ),
		                                      Traits::OutputBufferStart ( buffer ),
		                                      Traits::OutputBufferLength( buffer ),
		                                      toType );
		
		Traits::ReleaseOutputBuffer( buffer );
		
		return result;
	}
	
	template < ::DescType type >
	Nucleus::Owned< AEDesc > AECreateDesc( typename DescType_Traits< type >::Parameter data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		Nucleus::Owned< AEDesc > desc = AECreateDesc( DescType( type ),
		                                     Traits::OutputBufferStart ( buffer ),
		                                     Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
		
		return desc;
	}
	
	template < ::DescType type >
	void AEPutPtr( AEDescList&                                  list,
	               long                                         index,
	               typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutPtr( list,
		          index,
		          DescType( type ),
		          Traits::OutputBufferStart ( buffer ),
		          Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutPtr( Nucleus::Owned< AEDescList, Disposer >&               list,
	               long                                         index,
	               typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutPtr( list,
		          index,
		          DescType( type ),
		          Traits::OutputBufferStart ( buffer ),
		          Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetNthPtr( const AEDescList&  listDesc,
	             long               index )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
		{
			Traits::SetInputBufferLength( buffer,
			                              AESizeOfNthItem( listDesc, index ) );
		}
		
		AEGetNthPtr( listDesc,
		             index,
		             DescType( type ),
		             Traits::InputBufferStart ( buffer ),
		             Traits::InputBufferLength( buffer ) );
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutKeyPtr( AERecord&                                    record,
	                  AEKeyword                                    keyword,
	                  typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutKeyPtr( record,
		             keyword,
		             DescType( type ),
		             Traits::OutputBufferStart ( buffer ),
		             Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutKeyPtr( Nucleus::Owned< AERecord, Disposer >&                 record,
	                  AEKeyword                                    keyword,
	                  typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutKeyPtr( record,
		             keyword,
		             DescType( type ),
		             Traits::OutputBufferStart ( buffer ),
		             Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetKeyPtr( const AERecord&  record,
	             AEKeyword        keyword )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
		{
			Traits::SetInputBufferLength( buffer,
			                              AESizeOfKeyDesc( record, keyword ) );
		}
		
		AEGetKeyPtr( record,
		             keyword,
		             DescType( type ),
		             Traits::InputBufferStart ( buffer ),
		             Traits::InputBufferLength( buffer ) );
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutParamPtr( AppleEvent&                                  appleEvent,
	                    AEKeyword                                    keyword,
	                    typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutParamPtr( appleEvent,
		               keyword,
		               DescType( type ),
		               Traits::OutputBufferStart ( buffer ),
		               Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutParamPtr( Nucleus::Owned< AppleEvent, Disposer >&               appleEvent,
	                    AEKeyword                                    keyword,
	                    typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutParamPtr( appleEvent,
		               keyword,
		               DescType( type ),
		               Traits::OutputBufferStart ( buffer ),
		               Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetParamPtr( const AppleEvent&  appleEvent,
	               AEKeyword          keyword )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
		{
			Traits::SetInputBufferLength( buffer,
			                              AESizeOfParam( appleEvent, keyword ) );
		}
		
		AEGetParamPtr( appleEvent,
		               keyword,
		               DescType( type ),
		               Traits::InputBufferStart ( buffer ),
		               Traits::InputBufferLength( buffer ) );
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	void AEPutAttributePtr( AppleEvent&                                  appleEvent,
	                        AEKeyword                                    keyword,
	                        typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutAttributePtr( appleEvent,
		                   keyword,
		                   DescType( type ),
		                   Traits::OutputBufferStart ( buffer ),
		                   Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutAttributePtr( Nucleus::Owned< AppleEvent, Disposer >&               appleEvent,
	                        AEKeyword                                    keyword,
	                        typename DescType_Traits< type >::Parameter  data )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEPutAttributePtr( appleEvent,
		                   keyword,
		                   DescType( type ),
		                   Traits::OutputBufferStart ( buffer ),
		                   Traits::OutputBufferLength( buffer ) );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetAttributePtr( const AppleEvent&  appleEvent,
	                   AEKeyword          keyword )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
		{
			Traits::SetInputBufferLength( buffer,
			                              AESizeOfAttribute( appleEvent, keyword ) );
		}
		
		AEGetAttributePtr( appleEvent,
		                   keyword,
		                   DescType( type ),
		                   Traits::InputBufferStart ( buffer ),
		                   Traits::InputBufferLength( buffer ) );
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AEGetDescData( const AEDesc& desc )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::InputBuffer buffer;
		
		if ( Traits::inputHasVariableLength )
		{
			Traits::SetInputBufferLength( buffer, AEGetDescDataSize( desc ) );
		}
		
		AEGetDescData( desc,
		               Traits::InputBufferStart ( buffer ),
		               Traits::InputBufferLength( buffer ) );
		
		return Traits::ProcessInputBuffer( buffer );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AECoerceDescData( const AEDesc& desc )
	{
		return AEGetDescData< type >( AECoerceDesc( desc, type ) );
	}
	
	template < ::DescType type >
	void AEReplaceDescData( typename DescType_Traits< type >::Parameter  data,
	                        AEDesc&                                      result )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEReplaceDescData( DescType( type ),
		                   Traits::OutputBufferStart( buffer ),
		                   Traits::OutputBufferLength( buffer ),
		                   result );
		
		Traits::ReleaseOutputBuffer( buffer );
	}

	template < ::DescType type, class Disposer >
	void AEReplaceDescData( typename DescType_Traits< type >::Parameter  data,
	                        Nucleus::Owned< AEDesc, Disposer >&                   result )
	{
		typedef DescType_Traits< type > Traits;
		
		typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( data );
		
		AEReplaceDescData( DescType( type ),
		                   Traits::OutputBufferStart( buffer ),
		                   Traits::OutputBufferLength( buffer ),
		                   result );
		
		Traits::ReleaseOutputBuffer( buffer );
	}
	
}

#endif

